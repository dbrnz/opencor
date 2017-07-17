/*******************************************************************************

Copyright (C) The University of Auckland

OpenCOR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenCOR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/

//==============================================================================
// CVODES solver
//==============================================================================

#include "cvodessolver.h"

//==============================================================================



#include <cvodes/cvodes.h>
#include <cvodes/cvodes_band.h>
#include <cvodes/cvodes_bandpre.h>
#include <cvodes/cvodes_dense.h>
#include <cvodes/cvodes_diag.h>
#include <cvodes/cvodes_spbcgs.h>
#include "cvodes/cvodes_spgmr.h"
#include "cvodes/cvodes_sptfqmr.h"

#include <cvodes/cvodes_impl.h>

#include <nvector/nvector_serial.h>
#include <sundials/sundials_types.h>
#include <sundials/sundials_math.h>

//==============================================================================

namespace OpenCOR {
namespace CVODESSolver {

//==============================================================================



#define Ith(v,i)    NV_Ith_S(v,i-1)
#define ZERO    RCONST(0.0)






sensitivity_node* create_sensitivity_node(N_Vector* data,sensitivity_node* next){
        sensitivity_node* new_node = (sensitivity_node*)malloc(sizeof(sensitivity_node));
        new_node->data = data;
        new_node->next = next;
        return new_node;
}



int rhsFunction(double pVoi, N_Vector pStates, N_Vector pRates, void *pUserData)
{
    // Compute the RHS function

    CvodesSolverUserData *userData = static_cast<CvodesSolverUserData *>(pUserData);

    userData->computeRates()(pVoi, userData->constants(),
                             N_VGetArrayPointer_Serial(pRates),
                             N_VGetArrayPointer_Serial(pStates),
                             userData->algebraic());

    return 0;
}

//==============================================================================

void errorHandler(int pErrorCode, const char *pModule, const char *pFunction,
                  char *pErrorMessage, void *pUserData)
{
    Q_UNUSED(pModule);
    Q_UNUSED(pFunction);

    if (pErrorCode != CV_WARNING) {
        // CVODES generated an error, so forward it to the CvodesSolver object

        static_cast<CvodesSolver *>(pUserData)->emitError(pErrorMessage);
    }
}

//==============================================================================

CvodesSolverUserData::CvodesSolverUserData(double *pConstants, double *pAlgebraic,
                                         Solver::OdeSolver::ComputeRatesFunction pComputeRates) :
    mConstants(pConstants),
    mAlgebraic(pAlgebraic),
    mComputeRates(pComputeRates)
{
}

//==============================================================================

double * CvodesSolverUserData::constants() const
{
    // Return our constants array

    return mConstants;
}

//==============================================================================

double * CvodesSolverUserData::algebraic() const
{
    // Return our algebraic array

    return mAlgebraic;
}

//==============================================================================

Solver::OdeSolver::ComputeRatesFunction CvodesSolverUserData::computeRates() const
{
    // Return our compute rates function

    return mComputeRates;
}

//==============================================================================

CvodesSolver::CvodesSolver() :
    mSolver(0),
    mStatesVector(0),
    mUserData(0),
    mInterpolateSolution(InterpolateSolutionDefaultValue)
{
}

//==============================================================================

CvodesSolver::~CvodesSolver()
{
    // Make sure that the solver has been initialised

    if (!mSolver)
        return;

    // Delete some internal objects

    N_VDestroy_Serial(mStatesVector);

    N_VDestroyVectorArray_Serial(uS, NS);

    delete mUserData;

    CVodeFree(&mSolver);





    sensitivity_node* node = *uSs_head;
    sensitivity_node* new_node;
    while(node->data){
        new_node = node->next;
        for (int is=0;is<NS;is++){
            N_VDestroy_Serial(node->data[is]);
        }
        free(node->data);
        free(node);
        node=new_node;
    }




}

//==============================================================================

void CvodesSolver::initialize(const double &pVoiStart,
                             const int &pRatesStatesCount, double *pConstants,
                             double *pRates, double *pStates,
                             double *pAlgebraic,
                             ComputeRatesFunction pComputeRates)
{


    NS = this->mConstantsCount;


    if (!mSolver) {
        // Retrieve some of the CVODES properties

        double maximumStep = MaximumStepDefaultValue;
        int maximumNumberOfSteps = MaximumNumberOfStepsDefaultValue;
        QString integrationMethod = IntegrationMethodDefaultValue;
        QString iterationType = IterationTypeDefaultValue;
        QString linearSolver = LinearSolverDefaultValue;
        QString preconditioner = PreconditionerDefaultValue;
        int upperHalfBandwidth = UpperHalfBandwidthDefaultValue;
        int lowerHalfBandwidth = LowerHalfBandwidthDefaultValue;
        double relativeTolerance = RelativeToleranceDefaultValue;
        double absoluteTolerance = AbsoluteToleranceDefaultValue;

        if (mProperties.contains(MaximumStepId)) {
            maximumStep = mProperties.value(MaximumStepId).toDouble();
        } else {
            emit error(QObject::tr("the 'maximum step' property value could not be retrieved"));

            return;
        }

        if (mProperties.contains(MaximumNumberOfStepsId)) {
            maximumNumberOfSteps = mProperties.value(MaximumNumberOfStepsId).toInt();
        } else {
            emit error(QObject::tr("the 'maximum number of steps' property value could not be retrieved"));

            return;
        }

        if (mProperties.contains(IntegrationMethodId)) {
            integrationMethod = mProperties.value(IntegrationMethodId).toString();
        } else {
            emit error(QObject::tr("the 'integration method' property value could not be retrieved"));

            return;
        }

        if (mProperties.contains(IterationTypeId)) {
            iterationType = mProperties.value(IterationTypeId).toString();

            if (!iterationType.compare(NewtonIteration)) {
                // We are dealing with a Newton iteration, so retrieve and check
                // its linear solver

                if (mProperties.contains(LinearSolverId)) {
                    linearSolver = mProperties.value(LinearSolverId).toString();

                    bool needUpperAndLowerHalfBandwidths = false;

                    if (   !linearSolver.compare(DenseLinearSolver)
                        || !linearSolver.compare(DiagonalLinearSolver)) {
                        // We are dealing with a dense/diagonal linear solver,
                        // so nothing more to do
                    } else if (!linearSolver.compare(BandedLinearSolver)) {
                        // We are dealing with a banded linear solver, so we
                        // need both an upper and a lower half bandwidth

                        needUpperAndLowerHalfBandwidths = true;
                    } else {
                        // We are dealing with a GMRES/Bi-CGStab/TFQMR linear
                        // solver, so retrieve and check its preconditioner

                        if (mProperties.contains(PreconditionerId)) {
                            preconditioner = mProperties.value(PreconditionerId).toString();
                        } else {
                            emit error(QObject::tr("the 'preconditioner' property value could not be retrieved"));

                            return;
                        }

                        if (!preconditioner.compare(BandedPreconditioner)) {
                            // We are dealing with a banded preconditioner, so
                            // we need both an upper and a lower half bandwidth

                            needUpperAndLowerHalfBandwidths = true;
                        }
                    }

                    if (needUpperAndLowerHalfBandwidths) {
                        if (mProperties.contains(UpperHalfBandwidthId)) {
                            upperHalfBandwidth = mProperties.value(UpperHalfBandwidthId).toInt();

                            if (   (upperHalfBandwidth < 0)
                                || (upperHalfBandwidth >= pRatesStatesCount)) {
                                emit error(QObject::tr("the 'upper half-bandwidth' property must have a value between 0 and %1").arg(pRatesStatesCount-1));

                                return;
                            }
                        } else {
                            emit error(QObject::tr("the 'upper half-bandwidth' property value could not be retrieved"));

                            return;
                        }

                        if (mProperties.contains(LowerHalfBandwidthId)) {
                            lowerHalfBandwidth = mProperties.value(LowerHalfBandwidthId).toInt();

                            if (   (lowerHalfBandwidth < 0)
                                || (lowerHalfBandwidth >= pRatesStatesCount)) {
                                emit error(QObject::tr("the 'lower half-bandwidth' property must have a value between 0 and %1").arg(pRatesStatesCount-1));

                                return;
                            }
                        } else {
                            emit error(QObject::tr("the 'lower half-bandwidth' property value could not be retrieved"));

                            return;
                        }
                    }
                } else {
                    emit error(QObject::tr("the 'linear solver' property value could not be retrieved"));

                    return;
                }
            }
        } else {
            emit error(QObject::tr("the 'iteration type' property value could not be retrieved"));

            return;
        }

        if (mProperties.contains(RelativeToleranceId)) {
            relativeTolerance = mProperties.value(RelativeToleranceId).toDouble();

            if (relativeTolerance < 0) {
                emit error(QObject::tr("the 'relative tolerance' property must have a value greater than or equal to 0"));

                return;
            }
        } else {
            emit error(QObject::tr("the 'relative tolerance' property value could not be retrieved"));

            return;
        }

        if (mProperties.contains(AbsoluteToleranceId)) {
            absoluteTolerance = mProperties.value(AbsoluteToleranceId).toDouble();

            if (absoluteTolerance < 0) {
                emit error(QObject::tr("the 'absolute tolerance' property must have a value greater than or equal to 0"));

                return;
            }
        } else {
            emit error(QObject::tr("the 'absolute tolerance' property value could not be retrieved"));

            return;
        }

        if (mProperties.contains(InterpolateSolutionId)) {
            mInterpolateSolution = mProperties.value(InterpolateSolutionId).toBool();
        } else {
            emit error(QObject::tr("the 'interpolate solution' property value could not be retrieved"));

            return;
        }

        // Initialise the ODE solver itself

        OpenCOR::Solver::OdeSolver::initialize(pVoiStart, pRatesStatesCount,
                                               pConstants, pRates, pStates,
                                               pAlgebraic, pComputeRates);

        // Create the states vector

        mStatesVector = N_VMake_Serial(pRatesStatesCount, pStates);

        // Create the CVODE solver

        bool newtonIteration = !iterationType.compare(NewtonIteration);

        mSolver = CVodeCreate(!integrationMethod.compare(BdfMethod)?CV_BDF:CV_ADAMS,
                              newtonIteration?CV_NEWTON:CV_FUNCTIONAL);

        // Use our own error handler

        CVodeSetErrHandlerFn(mSolver, errorHandler, this);

        // Initialise the CVODE solver

        CVodeInit(mSolver, rhsFunction, pVoiStart, mStatesVector);

        // Set some user data

        mUserData = new CvodesSolverUserData(pConstants, pAlgebraic,
                                            pComputeRates);

        CVodeSetUserData(mSolver, mUserData);

        // Set the maximum step

        CVodeSetMaxStep(mSolver, maximumStep);

        // Set the maximum number of steps

        CVodeSetMaxNumSteps(mSolver, maximumNumberOfSteps);

        // Set the linear solver, if needed

        if (newtonIteration) {
            if (!linearSolver.compare(DenseLinearSolver)) {
                CVDense(mSolver, pRatesStatesCount);
            } else if (!linearSolver.compare(BandedLinearSolver)) {
                CVBand(mSolver, pRatesStatesCount, upperHalfBandwidth, lowerHalfBandwidth);
            } else if (!linearSolver.compare(DiagonalLinearSolver)) {
                CVDiag(mSolver);
            } else {
                // We are dealing with a GMRES/Bi-CGStab/TFQMR linear solver

                if (!preconditioner.compare(BandedPreconditioner)) {
                    if (!linearSolver.compare(GmresLinearSolver))
                        CVSpgmr(mSolver, PREC_LEFT, 0);
                    else if (!linearSolver.compare(BiCgStabLinearSolver))
                        CVSpbcg(mSolver, PREC_LEFT, 0);
                    else
                        CVSptfqmr(mSolver, PREC_LEFT, 0);

                    CVBandPrecInit(mSolver, pRatesStatesCount, upperHalfBandwidth, lowerHalfBandwidth);
                } else {
                    if (!linearSolver.compare(GmresLinearSolver))
                        CVSpgmr(mSolver, PREC_NONE, 0);
                    else if (!linearSolver.compare(BiCgStabLinearSolver))
                        CVSpbcg(mSolver, PREC_NONE, 0);
                    else
                        CVSptfqmr(mSolver, PREC_NONE, 0);
                }
            }
        }

        // Set the relative and absolute tolerances

        CVodeSStolerances(mSolver, relativeTolerance, absoluteTolerance);



        int* plist = (int*) malloc(NS * sizeof(int));
        for (int is=0;is<NS;is++) plist[is]=is;

        realtype* pbar = (realtype*) malloc(NS * sizeof(realtype));
        for (int is=0;is<NS;is++) pbar[is]=1;

        uS = N_VCloneVectorArray_Serial(NS, mStatesVector);
        for (int is=0;is<NS;is++)
            N_VConst(ZERO, uS[is]);

        CVodeSensInit1(mSolver, NS, CV_SIMULTANEOUS, NULL, uS);

        CVodeSensEEtolerances(mSolver);

        CVodeSetSensErrCon(mSolver,TRUE);

        CVodeSetSensDQMethod(mSolver, CV_CENTERED, ZERO);

        CVodeSetSensParams(mSolver, pConstants, pbar, plist);










    } else {
        // Reinitialise the CVODE object

        CVodeReInit(mSolver, pVoiStart, mStatesVector);
    }

    uSs_head = (sensitivity_node**) malloc(1*sizeof(sensitivity_node*));
    uSs_head[0] = create_sensitivity_node(NULL,NULL);

}

//==============================================================================

void CvodesSolver::solve(double &pVoi, const double &pVoiEnd) const
{
    // Solve the model



    if (!mInterpolateSolution)
        CVodeSetStopTime(mSolver, pVoiEnd);

    CVode(mSolver, pVoiEnd, mStatesVector, &pVoi, CV_NORMAL);

    CVodeGetSens(mSolver, &pVoi, uS);

    N_Vector* new_data = (N_Vector*) malloc(NS*sizeof(N_Vector));
    for (int is=0;is<NS;is++){
            new_data[is] =N_VClone(uS[is]);
    }
    sensitivity_node* new_head = create_sensitivity_node(new_data,*uSs_head);
    uSs_head[0] = new_head;



    // Compute the rates one more time to get up to date values for the rates
    // Note: another way of doing this would be to copy the contents of the
    //       calculated rates in rhsFunction, but that's bound to be more time
    //       consuming since a call to CVode() is likely to generate at least a
    //       few calls to rhsFunction(), so that would be quite a few memory
    //       transfers while here we 'only' compute the rates one more time...

    mComputeRates(pVoiEnd, mConstants, mRates,
                  N_VGetArrayPointer_Serial(mStatesVector), mAlgebraic);


}

//==============================================================================

}   // namespace CVODESSolver
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

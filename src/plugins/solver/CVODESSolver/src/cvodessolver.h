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

#pragma once

//==============================================================================

#include "solverinterface.h"

//==============================================================================

#include "nvector/nvector_serial.h"

//==============================================================================

namespace OpenCOR {
namespace CVODESSolver {

//==============================================================================

static const auto MaximumStepId          = QStringLiteral("MaximumStep");
static const auto MaximumNumberOfStepsId = QStringLiteral("MaximumNumberOfSteps");
static const auto IntegrationMethodId    = QStringLiteral("IntegrationMethod");
static const auto IterationTypeId        = QStringLiteral("IterationType");
static const auto LinearSolverId         = QStringLiteral("LinearSolver");
static const auto PreconditionerId       = QStringLiteral("Preconditioner");
static const auto UpperHalfBandwidthId   = QStringLiteral("UpperHalfBandwidth");
static const auto LowerHalfBandwidthId   = QStringLiteral("LowerHalfBandwidth");
static const auto RelativeToleranceId    = QStringLiteral("RelativeTolerance");
static const auto AbsoluteToleranceId    = QStringLiteral("AbsoluteTolerance");
static const auto InterpolateSolutionId  = QStringLiteral("InterpolateSolution");

//==============================================================================

static const auto AdamsMoultonMethod = QStringLiteral("Adams-Moulton");
static const auto BdfMethod          = QStringLiteral("BDF");

//==============================================================================

static const auto FunctionalIteration = QStringLiteral("Functional");
static const auto NewtonIteration     = QStringLiteral("Newton");

//==============================================================================

static const auto DenseLinearSolver    = QStringLiteral("Dense");
static const auto BandedLinearSolver   = QStringLiteral("Banded");
static const auto DiagonalLinearSolver = QStringLiteral("Diagonal");
static const auto GmresLinearSolver    = QStringLiteral("GMRES");
static const auto BiCgStabLinearSolver = QStringLiteral("BiCGStab");
static const auto TfqmrLinearSolver    = QStringLiteral("TFQMR");

//==============================================================================

static const auto NoPreconditioner     = QStringLiteral("None");
static const auto BandedPreconditioner = QStringLiteral("Banded");

//==============================================================================

// Default CVODES parameter values
// Note #1: a maximum step of 0 means that there is no maximum step as such and
//          that CVODES can use whatever step it sees fit...
// Note #2: CVODES's default maximum number of steps is 500 which ought to be big
//          enough in most cases...

static const double MaximumStepDefaultValue = 0.0;

enum {
    MaximumNumberOfStepsDefaultValue = 500
};

static const auto IntegrationMethodDefaultValue = BdfMethod;
static const auto IterationTypeDefaultValue = NewtonIteration;
static const auto LinearSolverDefaultValue = DenseLinearSolver;
static const auto PreconditionerDefaultValue = BandedPreconditioner;

enum {
    UpperHalfBandwidthDefaultValue = 0,
    LowerHalfBandwidthDefaultValue = 0
};

static const double RelativeToleranceDefaultValue = 1.0e-7;
static const double AbsoluteToleranceDefaultValue = 1.0e-7;

static const bool InterpolateSolutionDefaultValue = true;

//==============================================================================

class CvodesSolverUserData
{
public:
    explicit CvodesSolverUserData(double *pConstants, double *pAlgebraic,
                                 Solver::OdeSolver::ComputeRatesFunction pComputeRates);

    double * constants() const;
    double * algebraic() const;

    Solver::OdeSolver::ComputeRatesFunction computeRates() const;

private:
    double *mConstants;
    double *mAlgebraic;

    Solver::OdeSolver::ComputeRatesFunction mComputeRates;
};

//==============================================================================



struct sensitivity_node{
    N_Vector* data;
    struct sensitivity_node* next;
};

sensitivity_node* create_sensitivity_node(N_Vector* data,sensitivity_node* next);


class CvodesSolver : public Solver::OdeSolver
{
public:
    explicit CvodesSolver();
    ~CvodesSolver();

    virtual void initialize(const double &pVoiStart,
                            const int &pRatesStatesCount, double *pConstants,
                            double *pRates, double *pStates, double *pAlgebraic,
                            ComputeRatesFunction pComputeRates);

    virtual void solve(double &pVoi, const double &pVoiEnd) const;

private:
    void *mSolver;
    N_Vector mStatesVector;
    CvodesSolverUserData *mUserData;


    N_Vector* uS;
    int NS;
    sensitivity_node** uSs_head;


    bool mInterpolateSolution;
};

//==============================================================================

}   // namespace CVODESSolver
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

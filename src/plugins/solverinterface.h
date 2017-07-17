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
// Solver interface
//==============================================================================

#pragma once

//==============================================================================

#include "plugininfo.h"

//==============================================================================

#include <QList>
#include <QMetaType>
#include <QVariant>

//==============================================================================

extern "C" void doNonLinearSolve(char *pRuntime,
                                 void (*pFunction)(double *, double *, void *),
                                 double *pParameters, int *pRes, int pSize,
                                 void *pUserData);

//==============================================================================

namespace OpenCOR {
namespace Solver {

//==============================================================================

enum {
    SizeOfDouble = sizeof(double)
};

//==============================================================================

class Solver : public QObject
{
    Q_OBJECT

public:
    typedef QVariantMap Properties;

    explicit Solver();

    void setProperties(const Properties &pProperties);

    void emitError(const QString &pErrorMessage);

protected:
    Properties mProperties;

signals:
    void error(const QString &pErrorMessage);
};

//==============================================================================

class VoiSolver : public Solver
{
public:
    explicit VoiSolver();

    virtual void solve(double &pVoi, const double &pVoiEnd) const = 0;

    int mConstantsCount;

protected:
    int mRatesStatesCount;

    double *mConstants;
    double *mStates;
    double *mRates;
    double *mAlgebraic;
};

//==============================================================================

class OdeSolver : public VoiSolver
{
public:
    typedef int (*ComputeRatesFunction)(double pVoi, double *pConstants, double *pRates, double *pStates, double *pAlgebraic);

    explicit OdeSolver();

    virtual void initialize(const double &pVoiStart,
                            const int &pRatesStatesCount, double *pConstants,
                            double *pRates, double *pStates, double *pAlgebraic,
                            ComputeRatesFunction pComputeRates);

protected:
    ComputeRatesFunction mComputeRates;
};

//==============================================================================

class DaeSolver : public VoiSolver
{
public:
    typedef int (*ComputeEssentialVariablesFunction)(double pVoi, double *pConstants, double *pRates, double *pOldRates, double *pStates, double *pOldStates, double *pAlgebraic, double *pCondVar);
    typedef int (*ComputeResidualsFunction)(double pVoi, double *pConstants, double *pRates, double *pOldRates, double *pStates, double *pOldStates, double *pAlgebraic, double *pCondVar, double *pResId);
    typedef int (*ComputeRootInformationFunction)(double pVoi, double *pConstants, double *pRates, double *pOldRates, double *pStates, double *pOldStates, double *pAlgebraic, double *pCondVar);
    typedef int (*ComputeStateInformationFunction)(double *pStateInfo);

    explicit DaeSolver();
    ~DaeSolver();

    virtual void initialize(const double &pVoiStart, const double &pVoiEnd,
                            const int &pRatesStatesCount,
                            const int &pCondVarCount, double *pConstants,
                            double *pRates, double *pStates, double *pAlgebraic,
                            double *pCondVar,
                            ComputeEssentialVariablesFunction pComputeEssentialVariables,
                            ComputeResidualsFunction pComputeResiduals,
                            ComputeRootInformationFunction pComputeRootInformation,
                            ComputeStateInformationFunction pComputeStateInformation);

protected:
    int mCondVarCount;

    double *mOldRates;
    double *mOldStates;
    double *mCondVar;
};

//==============================================================================

class NlaSolver : public Solver
{
public:
    typedef void (*ComputeSystemFunction)(double *, double *, void *);

    explicit NlaSolver();

    virtual void initialize(ComputeSystemFunction pComputeSystem,
                            double *pParameters, int pSize,
                            void *pUserData = 0);

    virtual void solve() const = 0;

private:
    ComputeSystemFunction mComputeSystem;

    double *mParameters;
    int mSize;
    void *mUserData;
};

//==============================================================================

NlaSolver * nlaSolver(const QString &pRuntimeAddress);

void setNlaSolver(const QString &pRuntimeAddress, NlaSolver *pGlobalNlaSolver);
void unsetNlaSolver(const QString &pRuntimeAddress);

//==============================================================================

enum Type {
    Dae,
    Nla,
    Ode
};

//==============================================================================

class Property
{
public:
    enum Type {
        Boolean,
        Integer,
        Double,
        List
    };

    explicit Property(const Type &pType, const QString &pId,
                      const Descriptions &pDescriptions,
                      const QStringList &pListValues,
                      const QVariant &pDefaultValue,
                      const bool &pHasVoiUnit);

    Type type() const;
    QString id() const;
    Descriptions descriptions() const;
    QStringList listValues() const;
    QVariant defaultValue() const;
    bool hasVoiUnit() const;

private:
    Type mType;
    QString mId;
    Descriptions mDescriptions;
    QStringList mListValues;
    QVariant mDefaultValue;
    bool mHasVoiUnit;
};

//==============================================================================

typedef QList<Property> Properties;

//==============================================================================

}   // namespace Solver

//==============================================================================

extern "C" Q_DECL_EXPORT int solverInterfaceVersion();

//==============================================================================

class SolverInterface
{
public:
#define INTERFACE_DEFINITION
    #include "solverinterface.inl"
#undef INTERFACE_DEFINITION
};

//==============================================================================

typedef QList<SolverInterface *> SolverInterfaces;

//==============================================================================

}   // namespace OpenCOR

//==============================================================================

Q_DECLARE_INTERFACE(OpenCOR::SolverInterface, "OpenCOR::SolverInterface")

Q_DECLARE_METATYPE(OpenCOR::Solver::Solver::Properties)

//==============================================================================
// End of file
//==============================================================================

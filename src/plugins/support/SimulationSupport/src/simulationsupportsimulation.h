/*******************************************************************************

Copyright The University of Auckland

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*******************************************************************************/

//==============================================================================
// Simulation Support simulation
//==============================================================================

#pragma once

//==============================================================================

#include "datastoreinterface.h"
#include "simulationsupportsimulationworker.h"
#include "solverinterface.h"

//==============================================================================

#include <QObject>

//==============================================================================

namespace OpenCOR {

//==============================================================================

namespace CellMLSupport {
    class CellmlFileRuntime;
}   // namespace CellMLSupport

//==============================================================================

namespace SimulationSupport {

//==============================================================================

class SimulationSupportPlugin;
class SimulationSupportSimulation;

//==============================================================================

class SimulationSupportSimulationData : public QObject
{
    Q_OBJECT

public:
    explicit SimulationSupportSimulationData(SimulationSupportSimulation *pSimulation,
                                             const SolverInterfaces &pSolverInterfaces);
    ~SimulationSupportSimulationData();

    void update();

    SimulationSupportSimulation * simulation() const;

    double * constants() const;
    double * rates() const;
    double * states() const;
    double * algebraic() const;
    double * condVar() const;

    int delay() const;
    void setDelay(const int &pDelay);

    double startingPoint() const;
    void setStartingPoint(const double &pStartingPoint,
                          const bool &pRecompute = true);

    double endingPoint() const;
    void setEndingPoint(const double &pEndingPoint);

    double pointInterval() const;
    void setPointInterval(const double &pPointInterval);

    SolverInterface * odeSolverInterface() const;

    QString odeSolverName() const;
    void setOdeSolverName(const QString &pOdeSolverName);

    Solver::Solver::Properties odeSolverProperties() const;
    void addOdeSolverProperty(const QString &pName, const QVariant &pValue);

    SolverInterface * daeSolverInterface() const;

    QString daeSolverName() const;
    void setDaeSolverName(const QString &pDaeSolverName);

    Solver::Solver::Properties daeSolverProperties() const;
    void addDaeSolverProperty(const QString &pName, const QVariant &pValue);

    SolverInterface * nlaSolverInterface() const;

    QString nlaSolverName() const;
    void setNlaSolverName(const QString &pNlaSolverName,
                          const bool &pReset = true);

    Solver::Solver::Properties nlaSolverProperties() const;
    void addNlaSolverProperty(const QString &pName, const QVariant &pValue,
                              const bool &pReset = true);

    void reset(const bool &pInitialize = true);

    void recomputeComputedConstantsAndVariables(const double &pCurrentPoint,
                                                const bool &pInitialize = true);
    void recomputeVariables(const double &pCurrentPoint);

    bool isModified() const;
    void checkForModifications();

private:
    SimulationSupportSimulation *mSimulation;

    CellMLSupport::CellmlFileRuntime *mRuntime;

    SolverInterfaces mSolverInterfaces;

    int mDelay;

    double mStartingPoint;
    double mEndingPoint;
    double mPointInterval;

    QString mOdeSolverName;
    Solver::Solver::Properties mOdeSolverProperties;

    QString mDaeSolverName;
    Solver::Solver::Properties mDaeSolverProperties;

    QString mNlaSolverName;
    Solver::Solver::Properties mNlaSolverProperties;

    double *mConstants;
    double *mRates;
    double *mStates;
    double *mDummyStates;
    double *mAlgebraic;
    double *mCondVar;

    double *mInitialConstants;
    double *mInitialStates;

    void createArrays();
    void deleteArrays();

    SolverInterface * solverInterface(const QString &pSolverName) const;

signals:
    void updated(const double &pCurrentPoint);
    void modified(const bool &pIsModified);

    void error(const QString &pMessage);
};

//==============================================================================

class SimulationSupportSimulationResults : public QObject
{
    Q_OBJECT

public:
    explicit SimulationSupportSimulationResults(SimulationSupportSimulation *pSimulation);
    ~SimulationSupportSimulationResults();

    void update();

    bool reset(const bool &pCreateDataStore = true);

    void addPoint(const double &pPoint);

    qulonglong size() const;

    DataStore::DataStore * dataStore() const;

    double * points() const;

    double * constants(const int &pIndex) const;
    double * rates(const int &pIndex) const;
    double * states(const int &pIndex) const;
    double * algebraic(const int &pIndex) const;

private:
    SimulationSupportSimulation *mSimulation;

    CellMLSupport::CellmlFileRuntime *mRuntime;

    DataStore::DataStore *mDataStore;

    DataStore::DataStoreVariable *mPoints;

    DataStore::DataStoreVariables mConstants;
    DataStore::DataStoreVariables mRates;
    DataStore::DataStoreVariables mStates;
    DataStore::DataStoreVariables mAlgebraic;

    bool createDataStore();
    void deleteDataStore();

    QString uri(const QStringList &pComponentHierarchy, const QString &pName);
};

//==============================================================================

class SimulationSupportSimulation : public QObject
{
    Q_OBJECT

    friend class SimulationSupportSimulationWorker;

public:
    explicit SimulationSupportSimulation(CellMLSupport::CellmlFileRuntime *pRuntime);
    ~SimulationSupportSimulation();

    CellMLSupport::CellmlFileRuntime * runtime() const;

    QString fileName() const;

    SimulationSupportSimulationData * data() const;
    SimulationSupportSimulationResults * results() const;

    void update(CellMLSupport::CellmlFileRuntime *pRuntime);

    bool isRunning() const;
    bool isPaused() const;

    double currentPoint() const;

    int delay() const;
    void setDelay(const int &pDelay);

    double requiredMemory();

    double size();

    bool run();
    bool pause();
    bool resume();
    bool stop();

    bool reset();

private:
    SimulationSupportSimulationWorker *mWorker;

    CellMLSupport::CellmlFileRuntime *mRuntime;

    SimulationSupportSimulationData *mData;
    SimulationSupportSimulationResults *mResults;

    bool simulationSettingsOk(const bool &pEmitSignal = true);

signals:
    void running(const bool &pIsResuming);
    void paused();
    void stopped(const qint64 &pElapsedTime);

    void error(const QString &pMessage);
};

//==============================================================================

}   // namespace SimulationSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

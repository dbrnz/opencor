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
// Simulation Support functions
//==============================================================================

#include "cellmlfilemanager.h"
#include "combinefilemanager.h"
#include "filemanager.h"
#include "sedmlfilemanager.h"
#include "simulationsupport.h"
#include "simulationsupportplugin.h"

//==============================================================================

#include <QDir>
#include <QObject>
#include <QRegularExpression>

//==============================================================================

#include "sbmlapibegin.h"
    #include "sbml/math/ASTNode.h"
#include "sbmlapiend.h"

//==============================================================================

#include "sedmlapibegin.h"
    #include "sedml/SedAlgorithm.h"
    #include "sedml/SedDocument.h"
    #include "sedml/SedOneStep.h"
    #include "sedml/SedPlot2D.h"
    #include "sedml/SedRepeatedTask.h"
    #include "sedml/SedUniformTimeCourse.h"
    #include "sedml/SedVectorRange.h"
#include "sedmlapiend.h"

//==============================================================================

namespace OpenCOR {
namespace SimulationSupport {

//==============================================================================

QIcon parameterIcon(const CellMLSupport::CellmlFileRuntimeParameter::ParameterType &pParameterType)
{
    // Return an icon that illustrates the type of a parameter

    static const QIcon VoiIcon              = QIcon(":/SimulationSupport/voi.png");
    static const QIcon ConstantIcon         = QIcon(":/SimulationSupport/constant.png");
    static const QIcon ComputedConstantIcon = QIcon(":/SimulationSupport/computedConstant.png");
    static const QIcon RateIcon             = QIcon(":/SimulationSupport/rate.png");
    static const QIcon StateIcon            = QIcon(":/SimulationSupport/state.png");
    static const QIcon AlgebraicIcon        = QIcon(":/SimulationSupport/algebraic.png");
    static const QIcon ErrorNodeIcon        = QIcon(":/oxygen/emblems/emblem-important.png");

    switch (pParameterType) {
    case CellMLSupport::CellmlFileRuntimeParameter::Voi:
        return VoiIcon;
    case CellMLSupport::CellmlFileRuntimeParameter::Constant:
        return ConstantIcon;
    case CellMLSupport::CellmlFileRuntimeParameter::ComputedConstant:
        return ComputedConstantIcon;
    case CellMLSupport::CellmlFileRuntimeParameter::Rate:
        return RateIcon;
    case CellMLSupport::CellmlFileRuntimeParameter::State:
        return StateIcon;
    case CellMLSupport::CellmlFileRuntimeParameter::Algebraic:
        return AlgebraicIcon;
    default:
        // Not a relevant type, so return an error node icon
        // Note: we should never reach this point...

        return ErrorNodeIcon;
    }
}

//==============================================================================

static bool sedmlAlgorithmSupported(const libsedml::SedAlgorithm *pSedmlAlgorithm,
                                    SEDMLSupport::SedmlFileIssues &pSedmlFileIssues)
{
    // Make sure that we have an algorithm

    if (!pSedmlAlgorithm) {
        pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                         QObject::tr("only SED-ML files with one or two simulations with an algorithm are supported"));

        return false;
    }

    // Make sure that the given algorithm relies on an algorithm that we support

    SolverInterface *usedSolverInterface = 0;
    QString kisaoId = QString::fromStdString(pSedmlAlgorithm->getKisaoID());

    foreach (SolverInterface *solverInterface, SimulationSupportPlugin::instance()->solverInterfaces()) {
        if (!solverInterface->id(kisaoId).compare(solverInterface->solverName())) {
            usedSolverInterface = solverInterface;

            break;
        }
    }

    if (!usedSolverInterface) {
        pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                         QObject::tr("unsupported algorithm (%1)").arg(kisaoId));

        return false;
    }

    // Make sure that the algorithm parameters are also supported

    for (int i = 0, iMax = pSedmlAlgorithm->getNumAlgorithmParameters(); i < iMax; ++i) {
        QString kisaoId = QString::fromStdString(pSedmlAlgorithm->getAlgorithmParameter(i)->getKisaoID());
        QString id = usedSolverInterface->id(kisaoId);

        if (id.isEmpty() || !id.compare(usedSolverInterface->solverName())) {
            pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                             QObject::tr("unsupported algorithm parameter (%1)").arg(kisaoId));

            return false;
        }
    }

    // Make sure that the annotation, if any, contains at least the kind of
    // information we would expect

    libsbml::XMLNode *annotation = pSedmlAlgorithm->getAnnotation();

    if (annotation) {
        for (uint i = 0, iMax = annotation->getNumChildren(); i < iMax; ++i) {
            const XMLNode &node = annotation->getChild(i);

            if (   QString::fromStdString(node.getURI()).compare(SEDMLSupport::OpencorNamespace)
                || QString::fromStdString(node.getName()).compare(SEDMLSupport::SolverProperties)) {
                continue;
            }

            bool validSolverProperties = true;

            for (uint j = 0, jMax = node.getNumChildren(); j < jMax; ++j) {
                const XMLNode &solverPropertyNode = node.getChild(j);

                if (   QString::fromStdString(solverPropertyNode.getURI()).compare(SEDMLSupport::OpencorNamespace)
                    || QString::fromStdString(solverPropertyNode.getName()).compare(SEDMLSupport::SolverProperty)) {
                    continue;
                }

                int idIndex = solverPropertyNode.getAttrIndex(SEDMLSupport::SolverPropertyId.toStdString());
                int valueIndex = solverPropertyNode.getAttrIndex(SEDMLSupport::SolverPropertyValue.toStdString());

                if (   (idIndex == -1) || (valueIndex == -1)
                    || solverPropertyNode.getAttrValue(idIndex).empty()
                    || solverPropertyNode.getAttrValue(valueIndex).empty()) {
                    validSolverProperties = false;

                    break;
                }
            }

            if (!validSolverProperties) {
                pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                                 QObject::tr("incomplete algorithm annotation (missing algorithm property information)"));

                return false;
            }
        }
    }

    return true;
}

//==============================================================================

static bool sedmlFileSupported(SEDMLSupport::SedmlFile *pSedmlFile,
                               SEDMLSupport::SedmlFileIssues &pSedmlFileIssues)
{
    // Make sure that there is only one model

    libsedml::SedDocument *sedmlDocument = pSedmlFile->sedmlDocument();

    if (sedmlDocument->getNumModels() != 1) {
        pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                         QObject::tr("only SED-ML files with one model are supported"));

        return false;
    }

    // Make sure that the model is of CellML type

    libsedml::SedModel *model = sedmlDocument->getModel(0);
    QString language = QString::fromStdString(model->getLanguage());

    if (   language.compare(SEDMLSupport::Language::Cellml)
        && language.compare(SEDMLSupport::Language::Cellml_1_0)
        && language.compare(SEDMLSupport::Language::Cellml_1_1)) {
        pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                         QObject::tr("only SED-ML files with a CellML file are supported"));

        return false;
    }

    // Make sure that there is either one or two simulations

    int nbOfSimulations = sedmlDocument->getNumSimulations();

    if ((nbOfSimulations != 1) && (nbOfSimulations != 2)) {
        pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                         QObject::tr("only SED-ML files with one or two simulations are supported"));

        return false;
    }

    // Make sure that the first simulation is a uniform time course simulation

    libsedml::SedSimulation *firstSimulation = sedmlDocument->getSimulation(0);

    if (firstSimulation->getTypeCode() != libsedml::SEDML_SIMULATION_UNIFORMTIMECOURSE) {
        pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                         QObject::tr("only SED-ML files with a uniform time course as a (first) simulation are supported"));

        return false;
    }

    // Make sure that the initial time and output start time are the same, that
    // the output start time and output end time are different, and that the
    // number of points is greater than zero

    libsedml::SedUniformTimeCourse *uniformTimeCourse = static_cast<libsedml::SedUniformTimeCourse *>(firstSimulation);
    double initialTime = uniformTimeCourse->getInitialTime();
    double outputStartTime = uniformTimeCourse->getOutputStartTime();
    double outputEndTime = uniformTimeCourse->getOutputEndTime();
    int nbOfPoints = uniformTimeCourse->getNumberOfPoints();

    if (initialTime != outputStartTime) {
        pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                         QObject::tr("only SED-ML files with the same values for initialTime and outputStartTime are supported"));

        return false;
    }

    if (outputStartTime == outputEndTime) {
        pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Error,
                                                         QObject::tr("the values for outputStartTime and outputEndTime must be different"));

        return false;
    }

    if (nbOfPoints <= 0) {
        pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Error,
                                                         QObject::tr("the value for numberOfPoints must be greater than zero"));

        return false;
    }

    // Make sure that the algorithm used for the first simulation is supported

    if (!sedmlAlgorithmSupported(firstSimulation->getAlgorithm(), pSedmlFileIssues))
        return false;

    // Make sure that the annotation, if any, contains at least the kind of
    // information we would expect

    libsbml::XMLNode *firstSimulationAnnotation = firstSimulation->getAnnotation();

    if (firstSimulationAnnotation) {
        bool hasNlaSolver = false;

        for (uint i = 0, iMax = firstSimulationAnnotation->getNumChildren(); i < iMax; ++i) {
            const libsbml::XMLNode &node = firstSimulationAnnotation->getChild(i);

            if (   QString::fromStdString(node.getURI()).compare(SEDMLSupport::OpencorNamespace)
                || QString::fromStdString(node.getName()).compare(SEDMLSupport::NlaSolver)) {
                continue;
            }

            int nameIndex = node.getAttrIndex(SEDMLSupport::NlaSolverName.toStdString());

            if ((nameIndex != -1) && !node.getAttrValue(nameIndex).empty()) {
                if (hasNlaSolver) {
                    pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                                     QObject::tr("only one NLA solver is allowed"));

                    return false;
                } else {
                    hasNlaSolver = true;
                }
            } else {
                pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                                 QObject::tr("incomplete simulation annotation (missing NLA solver name)"));

                return false;
            }
        }
    }

    // Check whether there is a second simulation

    libsedml::SedSimulation *secondSimulation = sedmlDocument->getSimulation(1);

    if (secondSimulation) {
        // Make sure that the second simulation is a one-step simulation

        if (secondSimulation->getTypeCode() != libsedml::SEDML_SIMULATION_ONESTEP) {
            pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                             QObject::tr("only SED-ML files with a one-step as a second simulation are supported"));

            return false;
        }

        // Make sure that its step is greater than zero

        if (static_cast<libsedml::SedOneStep *>(secondSimulation)->getStep() <= 0) {
            pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Error,
                                                             QObject::tr("the value for step must be greater than zero"));

            return false;
        }

        // Make sure that its algorithm and annotation, if any, is the same as
        // for the first simulation

        std::stringstream firstStream;
        std::stringstream secondStream;
        libsbml::XMLOutputStream firstXmlStream(firstStream);
        libsbml::XMLOutputStream secondXmlStream(secondStream);

        firstSimulation->getAlgorithm()->write(firstXmlStream);

        if (secondSimulation->getAlgorithm())
            secondSimulation->getAlgorithm()->write(secondXmlStream);

        libsbml::XMLNode *secondSimulationAnnotation = secondSimulation->getAnnotation();

        if (firstSimulationAnnotation)
            firstSimulationAnnotation->write(firstXmlStream);

        if (secondSimulationAnnotation)
            secondSimulationAnnotation->write(secondXmlStream);

        if (firstStream.str().compare(secondStream.str())) {
            pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                             QObject::tr("only SED-ML files with two simulations with the same algorithm are supported"));

            return false;
        }
    }

    // Make sure that we have only one repeated task, which aim is to execute
    // each simulation (using a sub-task) once

    uint totalNbOfTasks = secondSimulation?3:2;

    if (sedmlDocument->getNumTasks() != totalNbOfTasks) {
        pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                         QObject::tr("only SED-ML files that execute one or two simulations once are supported"));

        return false;
    }

    libsedml::SedRepeatedTask *repeatedTask = 0;

    bool repeatedTaskOk = false;
    std::string repeatedTaskFirstSubTaskId = std::string();
    std::string repeatedTaskSecondSubTaskId = std::string();

    bool firstSubTaskOk = false;
    std::string firstSubTaskId = std::string();

    bool secondSubTaskOk = false;
    std::string secondSubTaskId = std::string();

    for (uint i = 0; i < totalNbOfTasks; ++i) {
        libsedml::SedTask *task = sedmlDocument->getTask(i);

        if (task->getTypeCode() == libsedml::SEDML_TASK_REPEATEDTASK) {
            // Make sure that the repeated task asks for the model to be reset,
            // that it has one range, no task change and one/two sub-task/s

            repeatedTask = static_cast<libsedml::SedRepeatedTask *>(task);

            if (    repeatedTask->getResetModel()
                &&  (repeatedTask->getNumRanges() == 1)
                && !repeatedTask->getNumTaskChanges()
                &&  (repeatedTask->getNumSubTasks() == totalNbOfTasks-1)) {
                // Make sure that the range is a vector range and that it's the
                // one referenced in the repeated task

                libsedml::SedRange *range = repeatedTask->getRange(0);

                if (    (range->getTypeCode() == libsedml::SEDML_RANGE_VECTORRANGE)
                    && !repeatedTask->getRangeId().compare(range->getId())) {
                    // Make sure that the vector range has one value that is
                    // equal to 1

                    libsedml::SedVectorRange *vectorRange = static_cast<libsedml::SedVectorRange *>(range);

                    if (   (vectorRange->getNumValues() == 1)
                        && (vectorRange->getValues().front() == 1)) {
                        // Make sure that the one/two sub-tasks have the correct
                        // order and retrieve their id

                        for (uint i = 0, iMax = totalNbOfTasks-1; i < iMax; ++i) {
                            libsedml::SedSubTask *subTask = repeatedTask->getSubTask(i);

                            if (subTask->getOrder() == 1)
                                repeatedTaskFirstSubTaskId = subTask->getTask();
                            else if (subTask->getOrder() == 2)
                                repeatedTaskSecondSubTaskId = subTask->getTask();
                        }

                        repeatedTaskOk = true;
                    }
                }
            }
        } else if (task->getTypeCode() == libsedml::SEDML_TASK) {
            // Make sure the sub-task references the correct model and
            // simulation

            if (   !task->getModelReference().compare(model->getId())
                && !task->getSimulationReference().compare(firstSimulation->getId())) {
                firstSubTaskOk = true;
                firstSubTaskId = task->getId();
            } else if (   secondSimulation
                       && !task->getModelReference().compare(model->getId())
                       && !task->getSimulationReference().compare(secondSimulation->getId())) {
                secondSubTaskOk = true;
                secondSubTaskId = task->getId();
            }
        }
    }

    if (   !repeatedTaskOk
        || !firstSubTaskOk || repeatedTaskFirstSubTaskId.compare(firstSubTaskId)
        || (     secondSimulation
            && (!secondSubTaskOk || repeatedTaskSecondSubTaskId.compare(secondSubTaskId)))) {
        pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                         QObject::tr("only SED-ML files that execute one or two simulations once are supported"));

        return false;
    }

    // Make sure that all the data generators have one variable that references
    // the repeated task, that follows the correct CellML format for their
    // target (and OpenCOR format for their degree, if any), and that is not
    // modified

    for (uint i = 0, iMax = sedmlDocument->getNumDataGenerators(); i < iMax; ++i) {
        libsedml::SedDataGenerator *dataGenerator = sedmlDocument->getDataGenerator(i);

        if ((dataGenerator->getNumVariables() != 1) || dataGenerator->getNumParameters()) {
            pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                             QObject::tr("only SED-ML files with data generators for one variable are supported"));

            return false;
        }

        libsedml::SedVariable *variable = dataGenerator->getVariable(0);

        if (variable->getSymbol().size() || variable->getModelReference().size()) {
            pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                             QObject::tr("only SED-ML files with data generators for one variable with a target and a task reference are supported"));

            return false;
        }

        if (variable->getTaskReference().compare(repeatedTask->getId())) {
            pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                             QObject::tr("only SED-ML files with data generators for one variable with a reference to a repeated task are supported"));

            return false;
        }

        static const QRegularExpression TargetStartRegEx  = QRegularExpression("^\\/cellml:model\\/cellml:component\\[@name='");
        static const QRegularExpression TargetMiddleRegEx = QRegularExpression("']\\/cellml:variable\\[@name='");
        static const QRegularExpression TargetEndRegEx    = QRegularExpression("'\\]$");

        bool referencingCellmlVariable = false;
        QString target = QString::fromStdString(variable->getTarget());

        if (target.contains(TargetStartRegEx) && target.contains(TargetEndRegEx)) {
            static const QString Separator = "|";

            target.remove(TargetStartRegEx);
            target.replace(TargetMiddleRegEx, Separator);
            target.remove(TargetEndRegEx);

            QStringList identifiers = target.split(Separator);

            if (identifiers.count() == 2) {
                static const QRegularExpression IdentifierRegEx = QRegularExpression("^[[:alpha:]_][[:alnum:]_]*$");

                QString componentName = identifiers.first();
                QString variableName = identifiers.last();

                referencingCellmlVariable =    IdentifierRegEx.match(componentName).hasMatch()
                                            && IdentifierRegEx.match(variableName).hasMatch();
            }
        }

        if (!referencingCellmlVariable) {
            pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                             QObject::tr("only SED-ML files with data generators for one variable with a reference to a CellML variable are supported"));

            return false;
        }

        libsbml::XMLNode *annotation = variable->getAnnotation();

        if (annotation) {
            for (uint i = 0, iMax = annotation->getNumChildren(); i < iMax; ++i) {
                const XMLNode &node = annotation->getChild(i);

                if (   QString::fromStdString(node.getURI()).compare(SEDMLSupport::OpencorNamespace)
                    || QString::fromStdString(node.getName()).compare(SEDMLSupport::VariableDegree)) {
                    continue;
                }

                bool validVariableDegree = false;

                if (node.getNumChildren() == 1) {
                    bool conversionOk;
                    int variableDegree = QString::fromStdString(node.getChild(0).getCharacters()).toInt(&conversionOk);

                    validVariableDegree = conversionOk && (variableDegree >= 0);
                }

                if (!validVariableDegree) {
                    pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                                     QObject::tr("only SED-ML files with data generators for one variable that is derived or not are supported"));

                    return false;
                }
            }
        }

        const libsbml::ASTNode *mathNode = dataGenerator->getMath();

        if (   (mathNode->getType() != libsbml::AST_NAME)
            || variable->getId().compare(mathNode->getName())) {
            pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                             QObject::tr("only SED-ML files with data generators for one variable that is not modified are supported"));

            return false;
        }
    }

    // Make sure that all the outputs are 2D outputs

    for (uint i = 0, iMax = sedmlDocument->getNumOutputs(); i < iMax; ++i) {
        libsedml::SedOutput *output = sedmlDocument->getOutput(i);

        if (output->getTypeCode() != libsedml::SEDML_OUTPUT_PLOT2D) {
            pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                             QObject::tr("only SED-ML files with 2D outputs are supported"));

            return false;
        }

        // Make sure that the curves reference listed data generators and don't
        // use logarithmic axes

        libsedml::SedPlot2D *plot = static_cast<libsedml::SedPlot2D *>(output);

        for (uint j = 0, jMax = plot->getNumCurves(); j < jMax; ++j) {
            libsedml::SedCurve *curve = plot->getCurve(j);

            if (curve->getLogX() || curve->getLogY()) {
                pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Information,
                                                                 QObject::tr("only SED-ML files with linear 2D outputs are supported"));

                return false;
            }

            if (   !sedmlDocument->getDataGenerator(curve->getXDataReference())
                || !sedmlDocument->getDataGenerator(curve->getYDataReference())) {
                pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Error,
                                                                 QObject::tr("a curve must reference existing data generators"));

                return false;
            }
        }
    }

    return true;
}

//==============================================================================

static bool combineArchiveSupported(COMBINESupport::CombineArchive *pCombineArchive,
                                    COMBINESupport::CombineArchiveIssues &pCombineArchiveIssues)
{
    // Load and make sure that our COMBINE archive is valid

    if (   !pCombineArchive->load()
        || !pCombineArchive->isValid(pCombineArchiveIssues)) {
        return false;
    }

    // Make sure that there is only one master file in our COMBINE archive

    if (pCombineArchive->masterFiles().count() != 1) {
        pCombineArchiveIssues << COMBINESupport::CombineArchiveIssue(COMBINESupport::CombineArchiveIssue::Information,
                                                                     QObject::tr("only COMBINE archives with one master file are supported"));

        return false;
    }

    return true;
}

//==============================================================================

static QString retrieveCellmlFile(const QString &pFileName,
                                  CellMLSupport::CellmlFile *&pCellmlFile,
                                  SEDMLSupport::SedmlFile *pSedmlFile,
                                  const FileType &pFileType,
                                  SEDMLSupport::SedmlFileIssues &pSedmlFileIssues,
                                  bool *pIsDirectOrIndirectRemoteFile)
{
    // Make sure that we support our SED-ML file

    if (!sedmlFileSupported(pSedmlFile, pSedmlFileIssues))
        return QString();

    // Retrieve the source of the CellML file, if any, referenced in our SED-ML
    // file

    QString modelSource = QString::fromStdString(pSedmlFile->sedmlDocument()->getModel(0)->getSource());

    // Check whether we are dealing with a local file (which location is
    // relative to that of our SED-ML file) or a remote file

    if (pIsDirectOrIndirectRemoteFile) {
        // We only want to determine whether we are dealing with a local file or
        // a remote one

        QString dummy;

        Core::checkFileNameOrUrl(modelSource, *pIsDirectOrIndirectRemoteFile, dummy);

        *pIsDirectOrIndirectRemoteFile = !*pIsDirectOrIndirectRemoteFile;
        // Note: since Core::checkFileNameOrUrl() tells us whether we are
        //       dealing with a local file...
    } else {
        Core::FileManager *fileManagerInstance = Core::FileManager::instance();
        QString url = fileManagerInstance->file(pFileName)->url();
        bool isLocalFile;
        QString dummy;

        Core::checkFileNameOrUrl(modelSource, isLocalFile, dummy);

        if (isLocalFile && url.isEmpty()) {
            // By default, our model source refers to a file name relative to
            // our SED-ML file

            QString cellmlFileName = Core::nativeCanonicalFileName(QFileInfo(pSedmlFile->fileName()).path()+QDir::separator()+modelSource);

#ifdef Q_OS_WIN
            // On Windows, if our model source exists, it means that it refers
            // to a file on a different drive rather than to a file name
            // relative to our SED-ML file

            if (QFile::exists(modelSource))
                cellmlFileName = modelSource;
#endif

            if (QFile::exists(cellmlFileName)) {
                pCellmlFile = new CellMLSupport::CellmlFile(cellmlFileName);

                // If possible, set our CellML file and its dependencies, if
                // any, as dependencies for our SED-ML file

                if (pFileType == SedmlFile) {
                    Core::FileManager::instance()->setDependencies(pFileName,
                                                                   QStringList() << pCellmlFile->fileName()
                                                                                 << pCellmlFile->dependencies(true));
                }
            } else {
                pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Error,
                                                                 QObject::tr("%1 could not be found").arg(modelSource));
            }
        } else {
            // Handle the case where our model source is a relative remote file

            static const QRegularExpression FileNameRegEx = QRegularExpression("/[^/]*$");

            if (isLocalFile)
                modelSource = url.remove(FileNameRegEx)+"/"+modelSource;

            // Retrieve the contents of our model source

            QString fileContents;
            QString errorMessage;

            if (Core::readFileContentsFromUrl(modelSource, fileContents, &errorMessage)) {
                // Save the contents of our model source to a local file and use
                // that to create a CellML file object after having asked our
                // file manager to manage it (so that CellML 1.1 files can be
                // properly instantiated)
                // Note: we also keep track of our model source's local file
                //       since we will need to unmanage it when closing this
                //       file...

                QString cellmlFileName = Core::temporaryFileName();

                if (Core::writeFileContentsToFile(cellmlFileName, fileContents)) {
                    fileManagerInstance->manage(cellmlFileName, Core::File::Remote, modelSource);

                    pCellmlFile = new CellMLSupport::CellmlFile(cellmlFileName);
                    return cellmlFileName;
                } else {
                    pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Error,
                                                                     QObject::tr("%1 could not be saved").arg(modelSource));
                }
            } else {
                pSedmlFileIssues << SEDMLSupport::SedmlFileIssue(SEDMLSupport::SedmlFileIssue::Error,
                                                                 QObject::tr("%1 could not be retrieved (%2)").arg(modelSource, Core::formatMessage(errorMessage)));
            }
        }
    }
    return QString();
}

//==============================================================================

static void retrieveSedmlFile(SEDMLSupport::SedmlFile *&pSedmlFile,
                              COMBINESupport::CombineArchive *pCombineArchive,
                              COMBINESupport::CombineArchiveIssues &pCombineArchiveIssues)
{
    // Make sure that we support our COMBINE archive

    if (!combineArchiveSupported(pCombineArchive, pCombineArchiveIssues))
        return;

    // Create a SED-ML file object for our COMBINE archive's master file

    pSedmlFile = new SEDMLSupport::SedmlFile(pCombineArchive->masterFiles().first().fileName());
}

//==============================================================================

QString retrieveFileDetails(const QString &pFileName,
                            CellMLSupport::CellmlFile *&pCellmlFile,
                            SEDMLSupport::SedmlFile *&pSedmlFile,
                            COMBINESupport::CombineArchive *&pCombineArchive,
                            FileType &pFileType,
                            SEDMLSupport::SedmlFileIssues &pSedmlFileIssues,
                            COMBINESupport::CombineArchiveIssues &pCombineArchiveIssues,
                            bool *pIsDirectOrIndirectRemoteFile)
{
    // Determine the type of file we are dealing with

    pCellmlFile = CellMLSupport::CellmlFileManager::instance()->cellmlFile(pFileName);
    pSedmlFile = pCellmlFile?0:SEDMLSupport::SedmlFileManager::instance()->sedmlFile(pFileName);
    pCombineArchive = pSedmlFile?0:COMBINESupport::CombineFileManager::instance()->combineArchive(pFileName);

    pFileType = pCellmlFile?CellmlFile:pSedmlFile?SedmlFile:CombineArchive;

    // In the case of a COMBINE archive, we need to retrieve the corresponding
    // SED-ML file while, in the case of a SED-ML file, we need to retrieve the
    // corresponding CellML file

    pSedmlFileIssues.clear();
    pCombineArchiveIssues.clear();

    if (pCombineArchive)
        retrieveSedmlFile(pSedmlFile, pCombineArchive, pCombineArchiveIssues);

    if (pSedmlFile) {
        return retrieveCellmlFile(pFileName, pCellmlFile, pSedmlFile, pFileType,
                                  pSedmlFileIssues, pIsDirectOrIndirectRemoteFile);
    }
    return QString();
}

//==============================================================================

}   // namespace SimulationSupport
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

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
// File manager
//==============================================================================

#include "corecliutils.h"
#include "coreguiutils.h"
#include "filemanager.h"

//==============================================================================

#include <QApplication>
#include <QFile>
#include <QTimer>
#include <QWindow>

//==============================================================================

namespace OpenCOR {
namespace Core {

//==============================================================================

FileManager::FileManager() :
    mFiles(QList<File *>()),
    mFileNameFiles(QMap<QString, File *>()),
    mFilesReadable(QMap<QString, bool>()),
    mFilesWritable(QMap<QString, bool>()),
    mCheckFilesEnabled(true)
{
    // Create our timer

    mTimer = new QTimer(this);

    // A connection to handle the timing out of our timer

    connect(mTimer, &QTimer::timeout,
            this, &FileManager::checkFiles);

    // Keep track of when OpenCOR gets/loses the focus

    if (qobject_cast<QGuiApplication *>(QCoreApplication::instance())) {
        connect(qApp, &QApplication::focusWindowChanged,
                this, &FileManager::focusWindowChanged);
    }
}

//==============================================================================

FileManager::~FileManager()
{
    // Delete some internal objects

    delete mTimer;

    // Remove all the managed files

    for (auto file : mFiles)
        delete file;
}

//==============================================================================

void FileManager::startStopTimer()
{
    // Start our timer if OpenCOR is active and we have files, or stop it if
    // either OpenCOR is not active or we don't have files anymore
    // Note #1: if we are to start our timer, then we check files first since
    //          waiting one second may seem long to a user (when some files have
    //          changed and after having reactivated OpenCOR)...
    // Note #2: checking files may result in a message box being shown and,
    //          therefore, in a focusWindowChanged() signal being emitted. To
    //          handle that signal would result in reentry, so we temporarily
    //          disable our handling of it...

    if (   !mTimer->isActive()
        &&  opencorActive() && !mFiles.isEmpty()) {
        disconnect(qApp, &QApplication::focusWindowChanged,
                   this, &FileManager::focusWindowChanged);

        checkFiles();

        connect(qApp, &QApplication::focusWindowChanged,
                this, &FileManager::focusWindowChanged);

        mTimer->start(1000);
    } else if (   mTimer->isActive()
               && (!opencorActive() || mFiles.isEmpty())) {
        mTimer->stop();
    }
}

//==============================================================================

FileManager * FileManager::instance()
{
    // Return the 'global' instance of our file manager class

    static FileManager instance;

    return static_cast<FileManager *>(globalInstance("OpenCOR::Core::FileManager::instance()",
                                                     &instance));
}

//==============================================================================

FileManager::Status FileManager::manage(const QString &pFileName,
                                        File::Type pType,
                                        const QString &pUrl)
{
    // Manage the given file, should it not be already managed

    QString fileName = canonicalFileName(pFileName);

    if (QFile::exists(fileName)) {
        if (file(fileName)) {
            return AlreadyManaged;
        } else {
            // The file isn't already managed, so add it to our list of managed
            // files and let people know about it being now managed

            File *file = new File(fileName, pType, pUrl);

            mFiles << file;

            mFileNameFiles.insert(fileName, file);

            startStopTimer();

            emit fileManaged(fileName);

            return Added;
        }
    } else {
        return DoesNotExist;
    }
}

//==============================================================================

FileManager::Status FileManager::unmanage(const QString &pFileName)
{
    // Unmanage the given file, should it be managed

    QString fileName = canonicalFileName(pFileName);
    File *file = FileManager::file(fileName);

    if (file) {
        // The file is managed, so we can remove it

        mFiles.removeOne(file);
        mFileNameFiles.remove(fileName);

        delete file;

        startStopTimer();

        emit fileUnmanaged(fileName);

        return Removed;
    } else {
        return NotManaged;
    }
}

//==============================================================================

File * FileManager::file(const QString &pFileName) const
{
    // Return the File object, if any, associated with the given file

    return mFileNameFiles.value(canonicalFileName(pFileName));
}

//==============================================================================

QString FileManager::sha1(const QString &pFileName) const
{
    // Return the SHA-1 value of the given file, should it be managed

    File *file = FileManager::file(canonicalFileName(pFileName));

    if (file)
        return file->sha1();
    else
        return QString();
}

//==============================================================================

void FileManager::reset(const QString &pFileName)
{
    // Reset the given file, should it be managed

    File *file = FileManager::file(canonicalFileName(pFileName));

    if (file)
        file->reset();
}

//==============================================================================

int FileManager::newIndex(const QString &pFileName) const
{
    // Return the given file's new index, if it is being managed

    File *file = FileManager::file(canonicalFileName(pFileName));

    if (file)
        return file->newIndex();
    else
        return 0;
}

//==============================================================================

QString FileManager::url(const QString &pFileName) const
{
    // Return the given file's URL, if it is being managed

    File *file = FileManager::file(canonicalFileName(pFileName));

    if (file)
        return file->url();
    else
        return QString();
}

//==============================================================================

QString FileManager::fileName(const QString &pUrl) const
{
    // Return the given file's file name, if it is being managed

    for (auto file : mFiles) {
        if (!pUrl.compare(file->url()))
            return file->fileName();
    }

    return QString();
}

//==============================================================================

bool FileManager::isDifferent(const QString &pFileName) const
{
    // Return whether the given file, if it is being managed, is different from
    // its corresponding physical version

    File *file = FileManager::file(canonicalFileName(pFileName));

    if (file)
        return file->isDifferent();
    else
        return false;
}

//==============================================================================

bool FileManager::isDifferent(const QString &pFileName,
                              const QString &pFileContents) const
{
    // Return whether the given file, if it is being managed, has the same
    // contents has the given one

    File *file = FileManager::file(canonicalFileName(pFileName));

    if (file)
        return file->isDifferent(pFileContents);
    else
        return false;
}

//==============================================================================

bool FileManager::isNew(const QString &pFileName) const
{
    // Return whether the given file, if it is being managed, is new

    File *file = FileManager::file(canonicalFileName(pFileName));

    if (file)
        return file->isNew();
    else
        return false;
}

//==============================================================================

bool FileManager::isRemote(const QString &pFileName) const
{
    // Return whether the given file, if it is being managed, is a remote one

    File *file = FileManager::file(canonicalFileName(pFileName));

    if (file)
        return file->isRemote();
    else
        return false;
}

//==============================================================================

bool FileManager::isModified(const QString &pFileName) const
{
    // Return whether the given file, if it is being managed, has been modified

    File *file = FileManager::file(canonicalFileName(pFileName));

    if (file)
        return file->isModified();
    else
        return false;
}

//==============================================================================

bool FileManager::isNewOrModified(const QString &pFileName) const
{
    // Return whether the given file is new or modified

    return isNew(pFileName) || isModified(pFileName);
}

//==============================================================================

bool FileManager::isLocalNewOrModified(const QString &pFileName) const
{
    // Return whether the given file is a local one, as well as is new or
    // modified

    return !isRemote(pFileName) && (isNew(pFileName) || isModified(pFileName));
}

//==============================================================================

void FileManager::makeNew(const QString &pFileName)
{
    // Make the given file new, should it be managed

    File *file = FileManager::file(canonicalFileName(pFileName));

    if (file) {
        QString fileName;

        if (newFile(fileName))
            file->makeNew(fileName);
    }
}

//==============================================================================

void FileManager::setModified(const QString &pFileName, bool pModified)
{
    // Set the modified state of the given file, should it be managed

    QString fileName = canonicalFileName(pFileName);
    File *file = FileManager::file(fileName);

    if (file && file->setModified(pModified))
        emit fileModified(fileName);
}

//==============================================================================

void FileManager::setDependenciesModified(const QString &pFileName,
                                          bool pModified)
{
    // Set the dependencies modified state of the given file, should it be
    // managed

    QString fileName = canonicalFileName(pFileName);
    File *file = FileManager::file(fileName);

    if (file)
        file->setDependenciesModified(pModified);
}

//==============================================================================

bool FileManager::isReadable(const QString &pFileName) const
{
    // Return whether the given file, if it is being managed, is readable

    File *file = FileManager::file(canonicalFileName(pFileName));

    if (file)
        return file->isReadable();
    else
        return false;
}

//==============================================================================

bool FileManager::isWritable(const QString &pFileName) const
{
    // Return whether the given file, if it is being managed, is writable

    File *File = file(canonicalFileName(pFileName));

    if (File)
        return File->isWritable();
    else
        return false;
}

//==============================================================================

bool FileManager::isReadableAndWritable(const QString &pFileName) const
{
    // Return whether the given file, if it is being managed, is readable and
    // writable

    return isReadable(pFileName) && isWritable(pFileName);
}

//==============================================================================

bool FileManager::isLocked(const QString &pFileName) const
{
    // Return whether the given file, if it is being managed, is locked

    File *file = FileManager::file(canonicalFileName(pFileName));

    if (file)
        return file->isLocked();
    else
        return false;
}

//==============================================================================

FileManager::Status FileManager::setLocked(const QString &pFileName,
                                           bool pLocked)
{
    // Set the locked status of the given file, should it be managed

    QString fileName = canonicalFileName(pFileName);
    File *file = FileManager::file(fileName);

    if (file) {
        File::Status status = file->setLocked(pLocked);

        if (status == File::LockedSet)
            emitFilePermissionsChanged(fileName);

        if (status == File::LockedNotNeeded)
            return LockedNotNeeded;
        else if (status == File::LockedSet)
            return LockedSet;
        else
            return LockedNotSet;
    } else {
        return NotManaged;
    }
}

//==============================================================================

QStringList FileManager::dependencies(const QString &pFileName) const
{
    // Return the given file's dependencies, should it be managed

    File *file = FileManager::file(canonicalFileName(pFileName));

    if (file)
        return file->dependencies();
    else
        return QStringList();
}

//==============================================================================

void FileManager::setDependencies(const QString &pFileName,
                                  const QStringList &pDependencies)
{
    // Set the dependencies of the given file, should it be managed

    File *file = FileManager::file(canonicalFileName(pFileName));

    if (file)
        file->setDependencies(pDependencies);
}

//==============================================================================

void FileManager::reload(const QString &pFileName)
{
    // Make sure that the given file is managed

    QString fileName = canonicalFileName(pFileName);
    File *file = FileManager::file(fileName);

    if (file) {
        // The file is managed, so reset its settings and let people know that
        // it should be reloaded

        file->reset();

        emit fileReloaded(fileName);

        // Reset our modified state and let people know about it, if needed

        if (file->setModified(false))
            emit fileModified(fileName);
    }
}

//==============================================================================

bool FileManager::newFile(QString &pFileName, const QByteArray &pContents)
{
    // Retrieve a temporary file name for our new file

    QString fileName = temporaryFileName();

    // Create a new file with the given contents

    if (writeFile(fileName, pContents)) {
        pFileName = fileName;

        return true;
    } else {
        pFileName = QString();

        return false;
    }
}

//==============================================================================

FileManager::Status FileManager::create(const QString &pUrl,
                                        const QByteArray &pContents)
{
    // Create a new file

    QString fileName;

    if (newFile(fileName, pContents)) {
        // Let people know that we have created a new file

        emit fileCreated(fileName, pUrl);

        return Created;
    } else {
        return NotCreated;
    }
}

//==============================================================================

FileManager::Status FileManager::create(const QString &pUrl,
                                        const QString &pContents)
{
    // Create a new file

    return create(pUrl, pContents.toUtf8());
}

//==============================================================================

FileManager::Status FileManager::rename(const QString &pOldFileName,
                                        const QString &pNewFileName)
{
    // Make sure that the given 'old' file is managed

    File *file = FileManager::file(pOldFileName);

    if (file) {
        // The 'old' file is managed, so rename it and let people know about it

        QString newFileName = canonicalFileName(pNewFileName);

        if (file->setFileName(newFileName)) {
            QString oldFileName = canonicalFileName(pOldFileName);

            mFileNameFiles.insert(newFileName, file);
            mFileNameFiles.remove(oldFileName);

            emit fileRenamed(oldFileName, newFileName);

            return Renamed;
        } else {
            return RenamingNotNeeded;
        }
    } else {
        return NotManaged;
    }
}

//==============================================================================

FileManager::Status FileManager::duplicate(const QString &pFileName)
{
    // Make sure that the given file is managed

    File *file = FileManager::file(canonicalFileName(pFileName));

    if (file) {
        // The file is managed, so retrieve its contents

        QByteArray fileContents;

        if (readFile(pFileName, fileContents)) {
            // Now, we can create a new file, which contents will be that of our
            // given file

            QString fileName;

            if (newFile(fileName, fileContents)) {
                // Let people know that we have duplicated a file

                emit fileDuplicated(fileName);

                return Duplicated;
            } else {
                return NotDuplicated;
            }
        } else {
            return NotDuplicated;
        }
    } else {
        return NotManaged;
    }
}

//==============================================================================

void FileManager::save(const QString &pFileName)
{
    // Make sure that the given file is managed

    QString fileName = canonicalFileName(pFileName);
    File *file = FileManager::file(fileName);

    if (file) {
        // The file is managed, so reset its settings and let people know that
        // it has been saved

        file->reset(false);

        emit fileSaved(fileName);
    }
}

//==============================================================================

int FileManager::count() const
{
    // Return the number of files currently being managed

    return mFiles.count();
}

//==============================================================================

void FileManager::emitFilePermissionsChanged(const QString &pFileName)
{
    // Update our internals and let people know that the given file has had its
    // permissions changed

    mFilesReadable.insert(pFileName, isReadable(pFileName));
    mFilesWritable.insert(pFileName, isWritable(pFileName));

    emit filePermissionsChanged(pFileName);
}

//==============================================================================

void FileManager::setCheckFilesEnabled(bool pCheckFilesEnabled)
{
    // Specify whether we can check files

    mCheckFilesEnabled = pCheckFilesEnabled;
}

//==============================================================================

void FileManager::focusWindowChanged()
{
    // Start/stop our timer

    startStopTimer();
}

//==============================================================================

void FileManager::checkFiles()
{
    // Make sure that OpenCOR is active
    // Note: indeed, although we try our best to enable/disable our timer as
    //       needed, there are cases (e.g. a QFileDialog is opened) that don't
    //       result in the focusWindowChanged() signal being emitted, which
    //       means that we can't enable/disable our timer in those acses, hence
    //       our checking that OpenCOR is really active indeed...

    if (!opencorActive() || !mCheckFilesEnabled)
        return;

    // Check our various files, after making sure that they are still being
    // managed
    // Note: indeed, some files may get added/removed while we are checking
    //       them, and to check a file that has been removed will crash
    //       OpenCOR...

    for (auto file : mFiles) {
        if (!mFiles.contains(file))
            continue;

        QString fileName = file->fileName();
        File::Status fileStatus = file->check();

        switch (fileStatus) {
        case File::Changed:
        case File::DependenciesChanged:
        case File::AllChanged:
            // The file and/or one or several of its dependencies has changed,
            // so let people know about it

            emit fileChanged(fileName,
                             (fileStatus == File::Changed) || (fileStatus == File::AllChanged),
                             (fileStatus == File::DependenciesChanged) || (fileStatus == File::AllChanged));

            break;
        case File::Unchanged:
            // The file has neither changed nor been deleted, so check whether
            // its permissions have changed

            if (    (mFilesReadable.value(fileName, false) != isReadable(fileName))
                ||  (mFilesWritable.value(fileName, false) != isWritable(fileName))
                || !(   mFilesReadable.contains(fileName)
                     && mFilesWritable.contains(fileName))) {
                emitFilePermissionsChanged(fileName);
            }

            break;
        case File::Deleted:
            // The file has been deleted, so let people know about it

            emit fileDeleted(fileName);

            break;
        default:
            // Not a relevant status, so do nothing

            ;
        }
    }
}

//==============================================================================

}   // namespace Core
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================

#include "mainwindow.h"
#include "editorwidget.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QAction>
#include <QMenu>
#include <QProcess>
#include <QToolBar>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    m_editor = new EditorWidget(this);
    setCentralWidget(m_editor);

    auto fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&New", QKeySequence::New, this, &MainWindow::newFile);
    fileMenu->addAction("&Open…", QKeySequence::Open, this, &MainWindow::openFile);
    fileMenu->addSeparator();
    fileMenu->addAction("&Save", QKeySequence::Save, this, &MainWindow::saveFile);
    fileMenu->addAction("Save &As…", QKeySequence::SaveAs, this, &MainWindow::saveFileAs);

    m_recentMenu = fileMenu->addMenu("Open &Recent");
    rebuildRecentMenu();
    fileMenu->addSeparator();
    fileMenu->addAction("&Exit", QKeySequence::Quit, this, &QWidget::close);

	auto buildBar = addToolBar("Build");
	buildBar->addAction("Build (Default)", this, &MainWindow::buildDefault);
	buildBar->addAction("Build Debug", this, &MainWindow::buildDebug);
	buildBar->addAction("Build Release", this, &MainWindow::buildRelease);
	buildBar->addAction("Build RelWithDebInfo", this, &MainWindow::buildRelWithDebInfo);

	m_buildDock = new QDockWidget("Build Output", this);
	m_buildOutput = new QPlainTextEdit(m_buildDock);
	m_buildOutput->setReadOnly(true);
	m_buildDock->setWidget(m_buildOutput);
	addDockWidget(Qt::BottomDockWidgetArea, m_buildDock);

    statusBar()->showMessage("Ready");
    resize(1000, 700);
    setWindowTitle("IDE");

    connect(m_editor, &EditorWidget::cursorPosChanged, this, &MainWindow::updateStatusLineCol);
    connect(m_editor, &EditorWidget::dirtyChanged, this, &MainWindow::updateWindowModified);
}

bool MainWindow::maybeSave() {
    if (!isWindowModified()) {
	return true;
    }
    auto decision = QMessageBox::question(this, "Unsaved changes",
        "Save changes before closing?",
        QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,
        QMessageBox::Yes);
    if (decision == QMessageBox::Cancel) {
	return false;
    }
    if (decision == QMessageBox::Yes) {
        if (m_editor->filePath().isEmpty()) {
            return doSaveAs(nullptr);
	}
        QString error;
        if (!m_editor->saveToFile(m_editor->filePath(), &error)) {
            QMessageBox::warning(this, "Save failed", error);
            return false;
        }
    }
    return true;
}

void MainWindow::closeEvent(QCloseEvent* ev) {
    if (maybeSave()) {
	ev->accept();
    }
    else {
	ev->ignore();
    }
}

void MainWindow::addToRecent(const QString& path) {
    m_recent.removeAll(path);
    m_recent.prepend(path);
    while (m_recent.size() > 10) {
	m_recent.removeLast();
    }
    rebuildRecentMenu();
}

void MainWindow::rebuildRecentMenu() {
    m_recentMenu->clear();
    for (const QString& path : m_recent) {
        QAction* action = m_recentMenu->addAction(path, this, &MainWindow::openRecent);
        action->setData(path);
    }
    if (m_recent.isEmpty()) {
        m_recentMenu->addAction("(empty)")->setEnabled(false);
    }
}

void MainWindow::updateStatusLineCol(int line, int col) {
    statusBar()->showMessage(QString("Ln %1, Col %2").arg(line).arg(col), 2000);
}

void MainWindow::updateWindowModified(bool dirty) {
    setWindowModified(dirty);
}

void MainWindow::newFile() {
    if (!maybeSave()) {
	return;
    }
    m_editor->setPlainText({});
    m_editor->setFilePath({});
}

void MainWindow::openFile() {
    if (!maybeSave()) {
	return;
    }
    QString path = QFileDialog::getOpenFileName(this, "Open file");
    if (path.isEmpty()) {
	return;
    }
    QString error;
    if (!m_editor->loadFromFile(path, &error)) {
        QMessageBox::warning(this, "Failed to open file", error);
        return;
    }
    addToRecent(path);
}

void MainWindow::saveFile() {
    if (m_editor->filePath().isEmpty()) {
        saveFileAs();
        return;
    }
    QString error;
    if (!m_editor->saveToFile(m_editor->filePath(), &error)) {
        QMessageBox::warning(this, "Save failed", error);
    }
}

bool MainWindow::doSaveAs(QString* outPath) {
    QString path = QFileDialog::getSaveFileName(this, "Save As");
    if (path.isEmpty()) {
	return false;
    }
    QString error;
    if (!m_editor->saveToFile(path, &error)) {
        QMessageBox::warning(this, "Save failed", error);
        return false;
    }
    addToRecent(path);
    if (outPath) {
	*outPath = path;
    }
    return true;
}

void MainWindow::saveFileAs() {
    doSaveAs(nullptr);
}

void MainWindow::openRecent() {
    auto action = qobject_cast<QAction*>(sender());
    if (!action) {
	return;
    }
    if (!maybeSave()) {
	return;
    }
    QString path = action->data().toString();
    if (path.isEmpty()) {
	return;
    }
    QString error;
    if (!m_editor->loadFromFile(path, &error)) {
        QMessageBox::warning(this, "Failed to open file", error);
    }
}

void MainWindow::buildDefault() {
	runBuild(QString());
}

void MainWindow::buildDebug() {
	runBuild("Debug");
}

void MainWindow::buildRelease() {
	runBuild("Release");
}

void MainWindow::buildRelWithDebInfo() {
	runBuild("RelWithDebInfo");
}

void MainWindow::runBuild(const QString& buildType) {
    QString buildDir = QFileDialog::getExistingDirectory(this, "Select Build Directory");
    QString program = "cmake";
    QStringList args = {"--build", buildDir};

    if (!buildType.isEmpty()) {
        args << "--config" << buildType;
    }
	m_buildOutput->clear();
    m_buildOutput->appendPlainText(QString("Running: %1 %2\n")
                          .arg(program, args.join(' ')));

    auto *proc = new QProcess(this);
	proc->setWorkingDirectory(buildDir);
    proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(proc, &QProcess::readyReadStandardOutput, this, [this, proc] {
		m_buildOutput->moveCursor(QTextCursor::End);
        m_buildOutput->insertPlainText(QString::fromLocal8Bit(proc->readAllStandardOutput()));
        m_buildOutput->moveCursor(QTextCursor::End);
    });
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, proc](int code, QProcess::ExitStatus status) {
        if (status == QProcess::NormalExit && code == 0) {
            m_buildOutput->appendPlainText("\nBuild finished successfully.");
        } else {
            m_buildOutput->appendPlainText("\nBuild failed");
        }
		proc->deleteLater();
    });

    proc->start(program, args);
    if (!proc->waitForStarted()) {
        m_buildOutput->appendPlainText("Could not start cmake \n");
        proc->deleteLater();
    }
}

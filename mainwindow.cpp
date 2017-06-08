#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Clear file name and set font color, icon
    QIcon icon(":/icon.png");
    this->setWindowIcon(icon);
    fileCount = 0;
    mName.clear();
    SetFontColor();

    // Communicate with UI class and GLWidget class
    SetupUI();
}

MainWindow::~MainWindow()
{
    delete ui;
    remove("mesh.obj");
}

void MainWindow::on_pushButtonOpen_clicked()
{
    QFileDialog* mDialog = new QFileDialog(this, tr("Open Mesh File"), "/Users/Jiaqing/Desktop", tr("Mesh Files (*.obj *.stl *.blender *.3ds *.ply *.off)"));
    mDialog->setAcceptMode(QFileDialog::AcceptOpen);
    if (mDialog->exec())
    {
        mName = mDialog->selectedFiles();
        ui->statusBar->showMessage(mName[0], 0);
        // Send a message to GLWidget for drawing the mesh
        emit CallDrawMeshFun(mName[0].toStdString());
        QFileInfo tmp(mName[0]);
        QString ss = tmp.fileName();
        ui->listWidgetOriginal->addItem(ss);
        inFileList.push_back(tmp);
        mName.clear();
    }
    delete mDialog;
}

void MainWindow::on_pushButtonCreate_clicked()
{
    QDir mdir = tr("MeshData");
    QFileInfo mFileInfo("mesh.obj");
    foreach(QFileInfo list, mdir.entryInfoList()) {
        if (list.fileName() == mFileInfo.fileName()) {
            fileCount++;
            QString tmp = mFileInfo.baseName()+ QString::number(fileCount, 10) + "." + mFileInfo.completeSuffix();
            QString rName = mdir.absolutePath() + "/" + tmp;
            emit CallNewMesh(rName.toStdString());
            ui->statusBar->showMessage(rName, 0);
            ui->listWidgetSegmented->addItem(tmp);
            return;
        }
    }
    QString str = mdir.absolutePath() + "/" + mFileInfo.fileName();
    emit CallNewMesh(str.toStdString());
    ui->statusBar->showMessage(str, 0);
    ui->listWidgetSegmented->addItem(mFileInfo.fileName());
}

void MainWindow::on_pushButtonSegment_clicked()
{
    // Send a message to GLWidget for drawing segmented mesh
    whichButton = 0;    // value is 0 if operation was segmentation
    emit CallDrawSegMeshFun(whichButton);
}

void MainWindow::on_pushButtonDelete_clicked()
{
    // Send a message to GLWidget for delete selected mesh data
    whichButton = 1;    // value is 1 if operation was deletion
    emit CallDrawDelMeshFun(whichButton);
}

bool MainWindow::event(QEvent *e)
{
    if (e->type() == QEvent::Show) {
        // Send a message to GLWidget for draw a ground
        emit CallDrawGroundFun();
        return true;
    }
    if (e->type() == QEvent::KeyPress) {
        QKeyEvent* key = static_cast<QKeyEvent*>(e);
        if (key->key() == Qt::Key_Delete) {
            if (ui->listWidgetOriginal->hasFocus() && ui->listWidgetOriginal->count() != 0) {
                QMessageBox::StandardButton obtn;
                QString str = "Are you sure to delete " + ui->listWidgetOriginal->currentItem()->text();
                obtn = QMessageBox::question(this, "Delete", str, QMessageBox::Yes | QMessageBox::No);
                if (obtn == QMessageBox::Yes)
                    ui->listWidgetOriginal->takeItem(ui->listWidgetOriginal->currentIndex().row());
            }
            if (ui->listWidgetSegmented->hasFocus() && ui->listWidgetSegmented->count() != 0) {
                QMessageBox::StandardButton btn;
                QString str = "Are you sure to delete " + ui->listWidgetSegmented->currentItem()->text();
                btn = QMessageBox::question(this, "Delete", str, QMessageBox::Yes | QMessageBox::No);
                if (btn == QMessageBox::Yes)
                    ui->listWidgetSegmented->takeItem(ui->listWidgetSegmented->currentIndex().row());
            }
        }
    }
    return QWidget::event(e);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasFormat("text/uri-list")) {
        e->setDropAction(Qt::MoveAction);
        e->acceptProposedAction();
    }
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *e)
{
    e->accept();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *e)
{
    e->setDropAction(Qt::MoveAction);
    e->accept();
}

void MainWindow::dropEvent(QDropEvent *e)
{
    QString str = e->mimeData()->text();
    str.remove(0, 8);       // Delete the head of path, 'file:///'.
    QFileInfo tmp(str);
    QString ss = tmp.fileName();
    ui->listWidgetOriginal->addItem(ss);
    inFileList.push_back(tmp);
    e->accept();
}

void MainWindow::on_listWidgetOriginal_itemDoubleClicked(QListWidgetItem *item)
{
    for (int i = 0; i < inFileList.size(); ++i) {
        if (inFileList.at(i).fileName() == item->text()) {
            if (inFileList.at(i).completeSuffix() == "jpg") {
                ui->statusBar->showMessage("!!!!! This is jpg file NOT 3D Model file !!!!!", 0);
                QString str = "!!!!! This is a jpg file NOT 3D Model file !!!!!";
                QMessageBox::warning(this, "Warning", str, QMessageBox::Ok, QMessageBox::NoButton);
                return;
            }
            emit CallDrawMeshFun(inFileList.at(i).filePath().toStdString());
            ui->statusBar->showMessage(inFileList.at(i).filePath(), 0);
        }
    }
}

void MainWindow::on_listWidgetSegmented_itemDoubleClicked(QListWidgetItem *item)
{
    QFileInfo mFileInfo(item->text());
    QString str = mFileInfo.absolutePath() + "/MeshData/" + item->text();
    emit CallDrawMeshFun(str.toStdString());
    ui->statusBar->showMessage(str, 0);
}

void MainWindow::on_pushButtonScan_clicked()
{
    QDesktopServices::openUrl(QUrl("file:///C:/Program Files/Skanect 1.7/bin/skanect.exe", QUrl::TolerantMode));
}

void MainWindow::on_pushButtonCombine_clicked()
{
    // Create path file for blender importing the selected models
    QDir mdir;
    QString tmp;
    QString path = mdir.absolutePath() + "/Scripts/";
    QString fname = path + "paths.txt";
    ofstream ofs;
    ofs.open(fname.toStdString(), ofstream::out);

    int start = 0;
    int end = ui->listWidgetSegmented->count();

    while (start != end) {
        QListWidgetItem* item = ui->listWidgetSegmented->item(start);
        QFileInfo mfi(item->text());
        tmp = mfi.absolutePath() + "/MeshData/" + item->text();
        ofs << tmp.toStdString() << endl;
        start++;
    }
    ofs.close();
    QDesktopServices::openUrl(QUrl("link.blend", QUrl::TolerantMode));
}

void MainWindow::SetFontColor()
{
    QFont font = ui->labelX->font();
    font.setPointSize(13);
    ui->labelX->setFont(font);
    ui->labelY->setFont(font);
    ui->labelZ->setFont(font);
    ui->labelDefX->setFont(font);
    ui->labelDefY->setFont(font);
    ui->labelDefZ->setFont(font);

    QPalette palette = ui->labelX->palette();
    palette.setColor(ui->labelX->foregroundRole(), Qt::red);
    ui->labelX->setPalette(palette);
    palette.setColor(ui->labelY->foregroundRole(), Qt::green);
    ui->labelY->setPalette(palette);
    palette.setColor(ui->labelZ->foregroundRole(), Qt::blue);
    ui->labelZ->setPalette(palette);
    palette.setColor(ui->labelDefX->foregroundRole(), Qt::red);
    ui->labelDefX->setPalette(palette);
    palette.setColor(ui->labelDefY->foregroundRole(), Qt::green);
    ui->labelDefY->setPalette(palette);
    palette.setColor(ui->labelDefZ->foregroundRole(), Qt::blue);
    ui->labelDefZ->setPalette(palette);
}

void MainWindow::SetupUI()
{
    connect(this, SIGNAL(CallDrawGroundFun()), ui->OpenGLWidget, SLOT(DrawGround()));
    connect(this, SIGNAL(CallDrawMeshFun(string)), ui->OpenGLWidget, SLOT(DrawMesh(string)));
    connect(this, SIGNAL(CallDrawSegMeshFun(int)), ui->OpenGLWidget, SLOT(DrawSegMesh(int)));
    connect(this, SIGNAL(CallDrawDelMeshFun(int)), ui->OpenGLWidget, SLOT(DrawDeleteMesh(int)));
    connect(this, SIGNAL(CallNewMesh(string)), ui->OpenGLWidget, SLOT(NewMesh(string)));
    connect(this, SIGNAL(CallClearRect()), ui->OpenGLWidget, SLOT(ClearMouseRect()));
    connect(ui->horizontalSliderX, SIGNAL(valueChanged(int)), ui->OpenGLWidget, SLOT(SetXTranslate(int)));
    connect(ui->horizontalSliderY, SIGNAL(valueChanged(int)), ui->OpenGLWidget, SLOT(SetYTranslate(int)));
    connect(ui->horizontalSliderZ, SIGNAL(valueChanged(int)), ui->OpenGLWidget, SLOT(SetZTranslate(int)));
    connect(ui->checkBoxWireframe, SIGNAL(clicked(bool)), ui->OpenGLWidget, SLOT(SetWireframe(bool)));
    connect(ui->checkBoxShowGround, SIGNAL(clicked(bool)), ui->OpenGLWidget, SLOT(SetGround(bool)));
    connect(ui->radioButtonDef, SIGNAL(clicked(bool)), ui->OpenGLWidget, SLOT(SetDefaultSearch(bool)));
    connect(ui->radioButtonKdtree, SIGNAL(clicked(bool)), ui->OpenGLWidget, SLOT(SetKdtreeSearch(bool)));
    connect(ui->OpenGLWidget, SIGNAL(SentLoadTime(QString)), ui->lineEditLoading, SLOT(setText(QString)));
    connect(ui->OpenGLWidget, SIGNAL(SentSegTime(QString)), ui->lineEditSegmenting, SLOT(setText(QString)));
    connect(ui->OpenGLWidget, SIGNAL(SentNumVert(QString)), ui->lineEditVertices, SLOT(setText(QString)));
    connect(ui->OpenGLWidget, SIGNAL(SentNumNorm(QString)), ui->lineEditNormals, SLOT(setText(QString)));
    connect(ui->OpenGLWidget, SIGNAL(SentNumFace(QString)), ui->lineEditFaces, SLOT(setText(QString)));
    connect(ui->horizontalSliderDefX, SIGNAL(valueChanged(int)), ui->OpenGLWidget, SLOT(DeformMeshX(int)));
    connect(ui->horizontalSliderDefY, SIGNAL(valueChanged(int)), ui->OpenGLWidget, SLOT(DeformMeshY(int)));
    connect(ui->horizontalSliderDefZ, SIGNAL(valueChanged(int)), ui->OpenGLWidget, SLOT(DeformMeshZ(int)));
}

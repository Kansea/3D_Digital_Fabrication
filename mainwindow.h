#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "appunit.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:        // Private variables and functions
    /* @This variable for UI */
    Ui::MainWindow *ui;

    /* @This variable for drop file list */
    QList<QFileInfo> inFileList;
    int fileCount;

    /* @This variable for saving selected file name */
    QStringList     mName;

    /* @Define a variable for select which button to press */
    int             whichButton;

    /* @This function for custom events when UI was implemented */
    bool    event(QEvent* e);

    /* @This function for drop and drag event */
    void	dragEnterEvent(QDragEnterEvent* e);
    void	dragLeaveEvent(QDragLeaveEvent* e);
    void	dragMoveEvent(QDragMoveEvent* e);
    void	dropEvent(QDropEvent* e);

    /* @This function setup font and color for UI*/
    void    SetFontColor();

    /* @This function setup UI connects */
    void    SetupUI();

signals:        // Senders for QT object
    /* @Call DrawMeshFun functions from GLWidget class */
    void    CallDrawMeshFun(string str);

    /* @Call DrawGroundFun functions from GLWidget class */
    void    CallDrawGroundFun();

    /* @Call DrawSegMeshFun functions from GLWidget class */
    void    CallDrawSegMeshFun(int whichButton);

    /* @Call DeleteMeshFun functions from GLWidget class */
    void    CallDrawDelMeshFun(int whichButton);

    /* @Call NewMesh functions from GLWidget class */
    void    CallNewMesh(string str);

    /* @Call clear mouse rect function from GLWidget class*/
    void    CallClearRect();

private slots:      // Receiver for this QT object
    /* @This function implement when OpenButton was pressed*/
    void    on_pushButtonOpen_clicked();

    /* @This function implement when CreateButton was pressed */
    void    on_pushButtonCreate_clicked();

    /* @This function implement when SegmentButton was pressed*/

    void    on_pushButtonSegment_clicked();

    /* @This function implement when ResetButton was pressed*/
    void    on_pushButtonDelete_clicked();

    /* @This function call 3D scanner software */
    void on_pushButtonScan_clicked();

    /* @This function call Blender software */
    void on_pushButtonCombine_clicked();

    /* @This function implememt when list widget was double clicked */
    void on_listWidgetOriginal_itemDoubleClicked(QListWidgetItem *item);
    void on_listWidgetSegmented_itemDoubleClicked(QListWidgetItem *item);
};

#endif // MAINWINDOW_H

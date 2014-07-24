#ifndef OVERLAYDIALOGBOX_H
#define OVERLAYDIALOGBOX_H

#include <QWidget>
#include <QDialog>
#include <QMessageBox>
#include <QInputDialog>

class OverlayDialogBox : public QWidget
{
    Q_OBJECT
public:
    explicit OverlayDialogBox(QWidget *parent, QDialog * dialog);
    QMessageBox * msgBox()
    {
        return qobject_cast<QMessageBox *>(m_dialog);
    }
    QInputDialog * inputDialog()
    {
        return qobject_cast<QInputDialog *>(m_dialog);
    }
    QDialog * dialog()
    {
        return m_dialog;
    }

signals:
    void accepted();
    void rejected();
    void finished(int);
public slots:
public:
    int exec();
    QDialog * m_dialog;
};






//#ifndef MAINWINDOW_H
//#define MAINWINDOW_H

//#include <QMainWindow>
//#include "overlaydialogbox.h"
//#include <QResizeEvent>

//class MainWindow : public QMainWindow
//{
//    Q_OBJECT

//public:
//    MainWindow(QWidget *parent = 0);
//    ~MainWindow();
//public slots:
//    void resizeEvent(QResizeEvent *event);

//private:
//    OverlayDialogBox * m_overlay;
//};

//#endif // MAINWINDOW_H
//mainwindow.cpp

//#include "mainwindow.h"

//MainWindow::MainWindow(QWidget *parent)
//    : QMainWindow(parent)
//{
//    m_overlay = new OverlayDialogBox(this);
//}

//MainWindow::~MainWindow() { }

//void MainWindow::resizeEvent(QResizeEvent *event)
//{
//    m_overlay->resize(event->size());
//    event->accept();
//}

#endif // OVERLAYDIALOGBOX_H

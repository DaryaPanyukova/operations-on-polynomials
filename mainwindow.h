#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


struct term {
    term* prev = nullptr;
    term* next = nullptr;
    int variables[26] = {0};
    int coefficient;
};

struct polynomial {
    polynomial* next = nullptr;
    polynomial* prev = nullptr;
    term* first_term = nullptr;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_button_add_polynomial_clicked();
    void Message(QString&);
    void UpdateBase(QString&, bool);

    void on_button_delete_clicked();

    void on_button_point_clicked();

    void on_butto_derivative_clicked();

    void on_button_combine_clicked();

    void on_button_add_clicked();

    void on_button_subtract_clicked();

    void on_button_multiply_clicked();

    void on_button_roots_clicked();

    void on_button_divide_clicked();

    void on_button_save_clicked();

    void on_button_file_read_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

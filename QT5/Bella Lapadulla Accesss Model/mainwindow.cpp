#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bella.h"


MODELOUT Model("Admin", "12345678");
LEVEL SecurityS(1, "Secret");
LEVEL SecurityTS(1, "TopSecret");
LEVEL IntegrityS(2, "S");
LEVEL IntegrityTS(2, "TS");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->comboBox_5->addItems(Model.Get_Subjects());
    ui->pushButton->setDisabled(true);
    ui->comboBox->addItem("Secret");
    ui->comboBox_2->addItem("S");
    ui->comboBox_3->addItem("Secret");
    ui->comboBox_4->addItem("S");
    ui->comboBox->addItem("TopSecret");
    ui->comboBox_2->addItem("TS");
    ui->comboBox_3->addItem("TopSecret");
    ui->comboBox_4->addItem("TS");
    ui->comboBox_6->addItems(Model.Get_Objects());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    int Error;
    if (ui->comboBox->currentIndex() == 0){
        if (ui->comboBox_2->currentIndex() == 0)
            Error = Model.Add_Subject(ui->lineEdit->text().toStdString(), ui->lineEdit_3->text().toStdString(), IntegrityS, SecurityS);
        if (ui->comboBox_2->currentIndex() == 1)
            Error = Model.Add_Subject(ui->lineEdit->text().toStdString(), ui->lineEdit_3->text().toStdString(), IntegrityTS, SecurityS);
    }
    if (ui->comboBox->currentIndex() == 1){
        if (ui->comboBox_2->currentIndex() == 0)
            Error = Model.Add_Subject(ui->lineEdit->text().toStdString(), ui->lineEdit_3->text().toStdString(), IntegrityS, SecurityTS);
        if (ui->comboBox_2->currentIndex() == 1)
            Error = Model.Add_Subject(ui->lineEdit->text().toStdString(), ui->lineEdit_3->text().toStdString(), IntegrityTS, SecurityTS);
    }
    if (Error == 0){
        ui->comboBox_5->addItem(ui->lineEdit->text());
    }
    if (Error == -1){
        QMessageBox::warning(this, "Error", "У вас нет прав для этого действия!");
    }
    ui->lineEdit->clear();
    ui->lineEdit_3->clear();
}

void MainWindow::on_pushButton_2_clicked()
{
    int Error;
    if (ui->comboBox_3->currentIndex() == 0){
        if (ui->comboBox_4->currentIndex() == 0)
            Error = Model.Add_Object(ui->lineEdit_2->text().toStdString(), IntegrityS, SecurityS);
        if (ui->comboBox_4->currentIndex() == 1)
            Error = Model.Add_Object(ui->lineEdit_2->text().toStdString(), IntegrityTS, SecurityS);
    }
    if (ui->comboBox->currentIndex() == 1){
        if (ui->comboBox_4->currentIndex() == 0)
            Error = Model.Add_Object(ui->lineEdit_2->text().toStdString(), IntegrityS, SecurityTS);
        if (ui->comboBox_4->currentIndex() == 1)
            Error = Model.Add_Object(ui->lineEdit_2->text().toStdString(), IntegrityTS, SecurityTS);
    }
    if (Error == -1){
         QMessageBox::warning(this, "Error", "Проверьте, возможно вы не прошли аутентификацию");
    }
    ui->comboBox_6->clear();
    ui->comboBox_6->addItems(Model.Get_Objects());
    ui->lineEdit_2->selectAll();
    ui->lineEdit_2->setFocus();
}

void MainWindow::on_pushButton_3_clicked()
{
    Model.Dell_Subject(ui->lineEdit_4->text().toStdString());
    ui->comboBox_5->clear();
    ui->comboBox_5->addItems(Model.Get_Subjects());
}

void MainWindow::on_pushButton_4_clicked()
{
    Model.Dell_Object(ui->lineEdit_5->text().toStdString());
    ui->comboBox_6->clear();
    ui->comboBox_6->addItems(Model.Get_Objects());
}

void MainWindow::on_pushButton_5_clicked()
{
    bool login = Model.Log_In(ui->comboBox_5->currentText().toStdString(), ui->lineEdit_6->text().toStdString());
    if (login == 0){
        QMessageBox::warning(this, "Error", "Ошибка аутентификации");
    }
    if (ui->comboBox_5->currentIndex() == 0){
        ui->pushButton->setEnabled(true);
    }
    else {
        ui->pushButton->setDisabled(true);
    }
}

void MainWindow::on_pushButton_6_clicked()
{
    int Rule;
    if (ui->radioButton->isChecked() == true){
        Rule = Model.Read_Object(ui->comboBox_6->currentText().toStdString());
        if (Rule == 0){
            QMessageBox::information(this, "Право", "Вы можете читать этот объект");
        }
    }
    if (ui->radioButton_2->isChecked() == true){
        Rule = Model.Write_Object(ui->comboBox_6->currentText().toStdString());
        if (Rule == 0){
            QMessageBox::information(this, "Право", "Вы можете писать этот объект");
        }
    }
    if (Rule != 0){
        QMessageBox::warning(this, "Error", "Произошла ошибка");
    }
}

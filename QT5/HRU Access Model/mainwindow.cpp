#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>


#include "hru.h"
MATRIXOUT Matrix("Admin", "12345678");
QString da;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->comboBox->addItems(Matrix.getSubjects());
    ui->pushButton_2->setDisabled(true);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
     int LoginIndex = ui->comboBox->currentIndex();
     QString Password = ui->lineEdit->text();
     if (Matrix.logIn(LoginIndex, Password) == 0){
         if (LoginIndex == 0){
             ui->pushButton_2->setEnabled(true);
             ui->tableWidget->setRowCount(Matrix.getObjectCount());
             ui->tableWidget->setColumnCount(Matrix.getSubjectCount());
             ui->tableWidget->setVerticalHeaderLabels(Matrix.getObjects());
             ui->tableWidget->setHorizontalHeaderLabels(Matrix.getSubjects());
             for (int i = 0; i < Matrix.getObjectCount(); i++){
                 for (int j = 0; j < Matrix.getSubjectCount(); j++){
                     ui->tableWidget->setItem(i, j, new QTableWidgetItem(Matrix.interpreteRules(j + 1, i + 1)));
                 }
             }
         }
         else{
             QStringList Obj;
             int k = 0;
             for (int i = 0; i < Matrix.getObjectCount(); i++){
                 if (Matrix.canRead(i + 1) == true){
                     Obj << Matrix.getObjects().at(i);
                     k++;
                 }
             }
             ui->tableWidget->clear();
             ui->tableWidget->setColumnCount(Matrix.getSubjectCount());
             //ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(ui->comboBox->currentText()));
             ui->tableWidget->setHorizontalHeaderLabels(Matrix.getSubjects());
             ui->tableWidget->setRowCount(k);
             ui->tableWidget->setVerticalHeaderLabels(Obj);
             int j = 0;
             do {
                 for (int i = 0; i < Matrix.getObjectCount(); i++){
                     if (Matrix.canRead(i + 1) == true){
                         ui->tableWidget->setItem(j, Matrix.getActiveSubject() - 1, new QTableWidgetItem(Matrix.interpreteRules(LoginIndex + 1, i + 1)));
                         j++;
                         continue;
                     }
                 }
             } while (j < k);
             ui->pushButton_2->setDisabled(true);
         }
     }
     else{
         ui->pushButton_2->setDisabled(true);
         QMessageBox::warning(this, "Error", "Ошибка аутентификации");
         ui->tableWidget->clear();
     }
     ui->lineEdit->clear();
}

void MainWindow::on_pushButton_2_clicked()
{
    int Error = Matrix.addSubject(ui->lineEdit_2->text(), ui->lineEdit_3->text());
    if (Error == 0){
        ui->comboBox->addItem(ui->lineEdit_2->text());
    }
    if (Error == -1){
        QMessageBox::warning(this, "Error", "У вас нет прав для этого действия!");
    }
    if (Error == 1){
        QMessageBox::critical(this, "Error", "Такой субъект уже существует");
    }
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
}

void MainWindow::on_pushButton_3_clicked()
{
    int Error = Matrix.addObject(ui->lineEdit_4->text());
    if (Error == -1){
        QMessageBox::warning(this, "Error", "Проверьте, возможно вы не авторизированны");
    }
    if (Error == 1){
        QMessageBox::critical(this, "Error", "Такой объект уже существует");
    }
    ui->lineEdit_4->selectAll();
    ui->lineEdit_4->setFocus();
}

void MainWindow::on_pushButton_4_clicked()
{
    if (Matrix.getActiveSubject() == 1){
        ui->tableWidget->setRowCount(Matrix.getObjectCount());
        ui->tableWidget->setColumnCount(Matrix.getSubjectCount());
        ui->tableWidget->setVerticalHeaderLabels(Matrix.getObjects());
        ui->tableWidget->setHorizontalHeaderLabels(Matrix.getSubjects());
        for (int i = 0; i < Matrix.getObjectCount(); i++){
            for (int j = 0; j < Matrix.getSubjectCount(); j++){
                ui->tableWidget->setItem(i, j, new QTableWidgetItem(Matrix.interpreteRules(j + 1, i + 1)));
            }
        }
    }
    else{
        QStringList Obj;
        int k = 0;
        for (int i = 0; i < Matrix.getObjectCount(); i++){
            if (Matrix.canRead(i + 1) == true){
                Obj << Matrix.getObjects().at(i);
                k++;
            }
        }
        ui->tableWidget->clear();
        ui->tableWidget->setColumnCount(Matrix.getSubjectCount());
        //ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(Matrix.getSubjects().at(Matrix.getActiveSubject() - 1)));
        ui->tableWidget->setHorizontalHeaderLabels(Matrix.getSubjects());
        ui->tableWidget->setRowCount(k);
        ui->tableWidget->setVerticalHeaderLabels(Obj);
        int j = 0;
        do {
            for (int i = 0; i < Matrix.getObjectCount(); i++){
                if (Matrix.canRead(i + 1) == true){
                    ui->tableWidget->setItem(j, Matrix.getActiveSubject() - 1, new QTableWidgetItem(Matrix.interpreteRules(Matrix.getActiveSubject(), i + 1)));
                    j++;
                    continue;
                }
            }
        } while (j < k);
    }
}

void MainWindow::on_tableWidget_cellChanged(int row, int column)
{
    QString RuleNew, RuleOld;
    RuleNew = ui->tableWidget->item(row, column)->text();
    RuleOld = Matrix.interpreteRules(column + 1, row + 1);
    if ((RuleNew.contains('r') == true) && (RuleOld.contains('r') == false)){
        Matrix.addRule(column + 1, row + 1, Read);
    }
    if ((RuleNew.contains('w') == true) && (RuleOld.contains('w') == false)){
        Matrix.addRule(column + 1, row + 1, Write);
    }
    if ((RuleNew.contains('o') == true) && (RuleOld.contains('o') == false)){
        if (Matrix.addRule(column + 1, row + 1, Own)== -1){
            QMessageBox::critical(this, "Error", "Ошибка");
        }
    }
    if ((RuleNew.contains('r') == false) && (RuleOld.contains('r') == true)){
        Matrix.dellRule(column + 1, row + 1, Read);
    }
    if ((RuleNew.contains('w') == false) && (RuleOld.contains('w') == true)){
        Matrix.dellRule(column + 1, row + 1, Write);
    }
    if ((RuleNew.contains('o') == false) && (RuleOld.contains('o') == true)){
       if (Matrix.dellRule(column + 1, row + 1, Own) == -1){
           QMessageBox::critical(this, "Error", "Ошибк");
       }
    }
}

void MainWindow::on_pushButton_5_clicked()
{

    int Error = Matrix.dellSubject(ui->lineEdit_5->text());
    if (Error == -1){
        QMessageBox::critical(this, "Error", "Ошибка");
    }
    else{
        ui->comboBox->clear();
        ui->comboBox->addItems(Matrix.getSubjects());
        ui->lineEdit_5->clear();
    }
}

void MainWindow::on_pushButton_6_clicked()
{
    int Error = Matrix.dellObject(ui->lineEdit_6->text());
    if (Error == -1){
        QMessageBox::critical(this, "Error", "Ошибка");
    }
    else {
        ui->lineEdit_6->clear();
    }
}

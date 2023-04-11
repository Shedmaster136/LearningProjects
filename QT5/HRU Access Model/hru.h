#ifndef HRU
#define HRU
#include <QVector> //реализаци на 2 мерном векторе (можно расширят и уменьшать и тд.
#include <QByteArray>
#include <QTextDocumentWriter>
#include <QString>

//Объект содежит в себа лишь имя и методы связанные с ним
//Как класс он нужен лишь потому что это ООП

class OBJECT
{
    public:
        OBJECT(QString myObjName);
        OBJECT(){}
        ~OBJECT(){}
        QString getName();
        void setName(QString myName);
    private:
        QString objName;
};

OBJECT::OBJECT(QString myObjName)
{
    objName = myObjName;
}
void OBJECT::setName(QString myName)
{
    objName = myName;
}

QString OBJECT::getName()
{
    return objName;
}
//================================================OBJECT
//Субъект содержит пароль и имя и методы связанные с ними
class SUBJECT
{
    public:
        SUBJECT(QString mySubjName, QString myPassword);
        SUBJECT(){}
        ~SUBJECT(){}
        QString getName(void);
        void setName(QString myName);
        void setPassword(QString myPassword);
        bool logIn(QString myPassword);//Возвращает true, если принятый пароль совпадает с фактическим.
    private:
        QString subjName;
        QString password;
};

bool SUBJECT::logIn(QString myPassword)
{
    if(myPassword == password)
        return true;
    return false;
}

SUBJECT::SUBJECT(QString mySubjName, QString myPassword)
{
    subjName = mySubjName;
    password = myPassword;
}
QString SUBJECT::getName(void)
{
    return subjName;
}
void SUBJECT::setName(QString myName)
{
    subjName = myName;
}

void SUBJECT::setPassword(QString myPassword)
{
    password = myPassword;
}

//=============================================================================SUBJECT
//Матрица - вектор векторов типа "байт". Права хранятся в виде 3 последних бит в конце каждого байта
//(0xb00000001 - own, 0xb000000010 - write, 0xb00000100 - read)
class MATRIX
{
    public:
        MATRIX(void);
        ~MATRIX(void);
        void addRow(void);//Добавить субъект
        void addColumn(void);//Добавить объект
        int dellRow(int myRow);//-1 - ошибка работы (невозможно получить доступ к переменной по данному адресу)
        int dellColumn(int myCoumn);//-1 - ошибка работы (невозможно получить доступ к переменной по данному адресу)
        quint8 getData(int myColumn, int myRow);//255 - ошибка работы (невозможно получить доступ к переменной по данному адресу)
        int setData(int myColumn, int myRow, quint8 myData);//-1 - ошибка работы (невозможно получить доступ к переменной по данному адресу)
    private:
        int rowSize;
        int columnSize;
        QVector<QByteArray> matrix;
        QVector<QByteArray>::iterator iteratorX;
};

//Создает матрицу размером 1х1
MATRIX::MATRIX(void)
{
    rowSize = 1;
    columnSize = 1;
    matrix.resize(rowSize);
    matrix[rowSize - 1].resize(columnSize);
    matrix[rowSize - 1][columnSize - 1] = 0;
    iteratorX = matrix.begin();
}
MATRIX::~MATRIX(void)
{
    matrix.~QVector();
}
void MATRIX::addRow(void)
{
    rowSize++;
    matrix.resize(rowSize);//Расширить матрицу до размеров rowSize
    matrix[rowSize-1].resize(columnSize);//расширить новый столбец до размера остальных столбцов
    matrix[rowSize-1].fill(0,-1);//и заполнить его нулями
}
void MATRIX::addColumn(void)
{
    columnSize++;
    iteratorX = matrix.begin();
    for(; iteratorX != matrix.end(); ++iteratorX)
    {
        iteratorX->append('\0');//добавление в конец каждого столбца существующей матрицы 0 (append не хочет принимать просто 0)
    }
}
int MATRIX::dellRow(int myRow)
{
    if(matrix.size() < myRow)//если ряд не счуществует, то вернуть ошибку
    {
        return -1;
    }
    rowSize--;
    matrix.removeAt(myRow - 1);//удаляет ряд с индексом myRow
    return 0;
}

int MATRIX::dellColumn(int myColumn)
{
    iteratorX = matrix.begin();
    if(iteratorX->size() < myColumn)
    {
        return -1;
    }
    columnSize--;
    for(; iteratorX != matrix.end(); ++iteratorX)
    {
        iteratorX->remove(myColumn - 1, 1);//удаляет байт с индексом myColumn в каждм ряду
    }
    return 0;
}

int MATRIX::setData(int myRow, int myColumn, quint8 myData)
{
   if((rowSize < myRow) || (columnSize < myColumn))//если такой элемент не существует, возвращает ошибку
       return -1;
    matrix[myRow - 1][myColumn - 1] = myData;
    return 0;
}
quint8 MATRIX::getData(int myRow, int myColumn)
{
    if((rowSize < myRow) || (columnSize < myColumn))//если такой элемент не существует, вернуть код ошибки
        return 255;
    return matrix[myRow - 1][myColumn - 1];
}
//=========================================================================MATRIX
#define Read 4//0xb00000100
#define Write 2//0xb00000010
#define Own 1//0xb00000001
#define All 7 //Дать все права 0xb00000111
class ACCESSMATRIX
{
    public:
        ACCESSMATRIX(QString adminName, QString adminPassword);
        ~ACCESSMATRIX(void);
        //Добавить право. Возвращает ошибку -1, если доступ получить нельзя, или невозможно
        int addRule(int mySubjNum, int myObjNum, quint8 myRule);
        //Удалить право. Возвращает ошибку -1, если доступ получить нельзя, или невозможно
        int dellRule(int mySubjNum, int myObjNum, quint8 myRule);
        //Добавить субъект. Возвращает ошибку -1, если доступ получить нельзя, или невозможно
        int addSubject(QString subjName, QString subjPassword);
        //Добавить объект. Возвращает ошибку -1, если доступ получить нельзя, или невозможно
        int addObject(QString objName);
        //Удалить субъект. Возвращает ошибку -1, если доступ получить нельзя, или невозможно
        int dellSubject(int mySubjNum);
        //Удалить объект. Возвращает ошибку -1, если доступ получить нельзя, или невозможно
        int dellObject(int myObjNum);
        //Возвращает true, если пароль верен и меняет activeSubject на logInSubj
        int logIn(int logInSubj, QString myPassword);
        //Возвращает true, если субъект с номером mySubjNum является владельцем объекта myObjNum
        bool isOwner(int mySubjNum,int myObjNum);
        //Возвращает true, если субъект с номером mySubjNum имеет право на чтение myObjNum
        bool canRead(int mySubjNum, int myObjNum);
        //Возвращает true, если субъект с номером mySubjNum имеет право на запись в myObjNum
        bool canWrite(int mySubjNum, int myObjNum);
        //Возвращает true, если субъект activeSubject является владельцем объекта myObjNum
        bool isOwner(int myObjNum);
        //Возвращает true, если субъект activeSubject имеет право на чтение myObjNum
        bool canRead(int myObjNum);
        //Возвращает true, если субъект activeSubject имеет право на запись в myObjNum
        bool canWrite(int myObjNum);
        //!!!Получить авторизированного ползователя
        int getActiveSubject(void);
        //!!Перегрузки для удобства в программе
        int dellSubject(QString SubjectName);
        int dellObject(QString ObjectName);
    protected:
        MATRIX accMatrix;//матрица доступа
        QVector<SUBJECT> subjects;//список субъектов
        QVector<OBJECT> objects;//список субъектов
    private:
        int activeSubject;//авторизованный в настоящий момент пользователь (-1 - никто не авторизован)
};
ACCESSMATRIX::~ACCESSMATRIX(void)
{
    accMatrix.~MATRIX();
    subjects.~QVector();
    objects.~QVector();
}
//Создается матрица 1х1 задается 1 субъект - админ.
ACCESSMATRIX::ACCESSMATRIX(QString adminName, QString adminPassword)
{
    subjects.resize(subjects.size() + 1);
    objects.resize(objects.size() + 1);
    subjects[0].setName(adminName);
    subjects[0].setPassword(adminPassword);
    activeSubject = -1;//активным субъектом усанавливается никто.
    objects[0].setName("Access matrix");
    accMatrix.setData(subjects.size(), objects.size(), All);
}
//добавлять правило для субъекта может только Админ, либо владелец объекта с номером myObjNum
int ACCESSMATRIX::addRule(int mySubjNum, int myObjNum, quint8 myRule)//done
{
    quint8 currValue;
    quint8 giveOwn;//=
    quint8 currentOwnerRights;//=
    int currentOwnerNum = -1;
    if(myRule > All)//поданый набор прав попросту не существует
        return -1;

    if(activeSubject == -1)//никто не авторизован
        return -1;
    if((activeSubject == 1) || (isOwner(activeSubject, myObjNum)))
    {
        if(myObjNum != 1)//нельзя передать права на матрицу
        {
            currValue =  accMatrix.getData(mySubjNum, myObjNum);//если данных с таким адресом не сужествует, вернет ошибку (255)
            if(currValue == 255)
                 return -1;
            giveOwn = myRule;
            giveOwn = giveOwn & Own;
            if(giveOwn == Own)//если пытаемся передать право овладевания, то удаляем его у себя
            {
               if(activeSubject == 1)
               {
                   for(int i = 1; i <= subjects.size(); i++)
                   {
                        currentOwnerRights = accMatrix.getData(i, myObjNum);
                        currentOwnerRights = currentOwnerRights & Own;
                        if(currentOwnerRights == Own)
                        {
                            currentOwnerNum = i;
                            break;
                        }
                   }
               }
               else
                   currentOwnerNum = activeSubject;
               if(currentOwnerNum != -1)
               {
                   currentOwnerRights = accMatrix.getData(currentOwnerNum, myObjNum);
                    currentOwnerRights = currentOwnerRights & (~Own);
                    accMatrix.setData(currentOwnerNum, myObjNum, currentOwnerRights);
               }
            }
            currValue = currValue | myRule;
            accMatrix.setData(mySubjNum, myObjNum, currValue);
            return 0;
        }
    }
    return -1;
}
//удалить право можо всем, если авторизован Админ, или если авторизованный является хозяином данного объекта, или самому себе
int ACCESSMATRIX::dellRule(int mySubjNum, int myObjNum, quint8 myRule)//==================редактировать
{
    quint8 currValue = 0;
    if(myRule > All)
        return -1;
    if(activeSubject == -1)
        return -1;
    if((activeSubject == 1) || (isOwner(activeSubject, myObjNum) || (mySubjNum == activeSubject)))
    {
            currValue = accMatrix.getData(mySubjNum, myObjNum);//если данных с таким адресом не сужествует, вернет ошибку (255)
            if(currValue == 255)
                return -1;
            currValue = currValue & (~myRule);
            accMatrix.setData(mySubjNum, myObjNum, currValue);
            return 0;
    }
    return -1;
}
//Создавать субъекты может только владелец матрицы доступа (Админ)
int ACCESSMATRIX::addSubject(QString subjName, QString subjPassword)
{
    //!!!!!!!--
    if(activeSubject != 1)
        return -1;
    ///!!!!!!
    for (int i = 0; i < subjects.size(); i++){
        if (subjects[i].getName() == subjName){
            return 1;
        }
    }
    subjects.resize(subjects.size() + 1);
    subjects[subjects.size() - 1].setName(subjName);
    subjects[subjects.size() - 1].setPassword(subjPassword);
    accMatrix.addRow();
    return 0;
}
//Cоздателю объекта присваиваются все права. Создавать объекты может любой субъект
//!!!!!
int ACCESSMATRIX::addObject(QString objName)
{
    if(activeSubject == -1)
        return -1;
    ///!!!!!
    for (int i = 0; i < objects.size(); i++){
        if (objects[i].getName() == objName){
            return 1;
        }
    }
    objects.resize(objects.size() + 1);
    objects[objects.size() - 1].setName(objName);
    accMatrix.addColumn();
    accMatrix.setData(activeSubject, objects.size(), All);//Вот тут была ошибка
    return 0;
}
//Админа нельзя удалить. Любой другой субъект можно удалить с записи админа, или самого субъекта.
int ACCESSMATRIX::dellSubject(int mySubjNum)//done
{
    int iResult = 0;//!!!результат выполнения dellRow
    if((activeSubject != mySubjNum) || (activeSubject == -1))
    {
        return -1;
    }
    if(mySubjNum == 1)
    {
        return -1;
    }
    if(activeSubject == mySubjNum)
    {
        activeSubject = -1;
    }
    iResult = accMatrix.dellRow(mySubjNum);//---
    if(iResult != -1)
    {
        subjects.removeAt(mySubjNum - 1);
    }
    return iResult;
}
//!!!!!
int ACCESSMATRIX::dellSubject(QString SubjectName)//done
{
    int iResult = -1;//!!!результат выполнения dellRow
    int mySubjNum = -1;
    for (int i = 0; i < subjects.size(); i++){
        if (subjects[i].getName() == SubjectName){
            mySubjNum = i + 1;
            break;
        }
    }
    if (activeSubject != 1){
        if((activeSubject != mySubjNum) || (activeSubject == -1))
        {
            return -1;
        }
        if((mySubjNum == 1) || (mySubjNum == -1))
        {
            return -1;
        }
    }
    if(activeSubject == mySubjNum)
    {
        activeSubject = -1;
    }
    iResult = accMatrix.dellRow(mySubjNum);//---
    if(iResult != -1)
    {
        subjects.removeAt(mySubjNum - 1);
    }
    return iResult;
}
//Саму матрицу доступа нельзя удалить.
//Удалить объект может либо его владелец, либо Админ
int ACCESSMATRIX::dellObject(int myObjNum)//done
{
    int iResult = -1;
    if(activeSubject == -1)
        return -1;
    if(myObjNum == 1)
    {
        return -1;
    }
    if((isOwner(activeSubject, myObjNum) == true) || (activeSubject == 1))
    {
        objects.removeAt(myObjNum - 1);
        iResult = accMatrix.dellColumn(myObjNum);
        if(iResult != -1)
        {
            objects.removeAt(myObjNum - 1);
        }
        return iResult;
    }
    return -1;
}
//!!!!!!!!!!!!!!
int ACCESSMATRIX::dellObject(QString ObjectName){
    int iResult = -1;
    int myObjNum = -1;
    for (int i = 0; i < objects.size(); i++){
        if (objects[i].getName() == ObjectName){
            myObjNum = i + 1;
            break;
        }
    }
    if(activeSubject == -1){
        return -1;
    }
    if(myObjNum == 1){
        return -1;
    }
    if((isOwner(activeSubject, myObjNum) == true) || (activeSubject == 1))
    {
        //objects.removeAt(myObjNum - 1);
        iResult = accMatrix.dellColumn(myObjNum);
        if(iResult != -1)
        {
            objects.removeAt(myObjNum - 1);
        }
        return iResult;
    }
    return -1;
}

int ACCESSMATRIX::logIn(int logInSubj, QString myPassword)
{
    //!!!!!!!
    if(subjects[logInSubj].logIn(myPassword) != true){
        activeSubject = -1;
        return -1;
    }
    activeSubject = logInSubj+1;
    return 0;
}

bool ACCESSMATRIX::isOwner(int mySubjNum, int myObjNum)
{
    quint8 accBits;
    quint8 accMask = Own;
    accBits = accMatrix.getData(mySubjNum, myObjNum);
    if(accBits == 255)
        return false;
    accBits = accBits & accMask;
    if(accBits == Own)
    {
        return true;
    }
    return(false);
}

bool ACCESSMATRIX::canRead(int mySubjNum, int myObjNum)
{
    quint8 accBits;
    quint8 accMask = Read;
    accBits = accMatrix.getData(mySubjNum, myObjNum);
    if(accBits == 255)
        return false;
    accBits = accBits & accMask;
    if(accBits == Read)
    {
        return true;
    }
    return(false);
}

bool ACCESSMATRIX::canWrite(int mySubjNum, int myObjNum)
{
    quint8 accBits;
    quint8 accMask = Write;
    accBits = accMatrix.getData(mySubjNum, myObjNum);
    if(accBits == 255)
        return false;
    accBits = accBits & accMask;
    if(accBits == Write)
    {
        return true;
    }
    return(false);
}
bool ACCESSMATRIX::isOwner(int myObjNum)
{
    quint8 accBits;
    quint8 accMask = Own;
    accBits = accMatrix.getData(activeSubject, myObjNum);
    if(accBits == 255)
        return false;
    accBits = accBits & accMask;
    if(accBits == Own)
    {
        return true;
    }
    return(false);
}

bool ACCESSMATRIX::canRead(int myObjNum)
{
    quint8 accBits;
    quint8 accMask = Read;
    accBits = accMatrix.getData(activeSubject, myObjNum);
    if(accBits == 255)
        return false;
    accBits = accBits & accMask;
    if(accBits == Read)
    {
        return true;
    }
    return(false);
}

bool ACCESSMATRIX::canWrite(int myObjNum)
{
    quint8 accBits;
    quint8 accMask = Write;
    accBits = accMatrix.getData(activeSubject, myObjNum);
    if(accBits == 255)
        return false;
    accBits = accBits & accMask;
    if(accBits == Write)
    {
        return true;
    }
    return(false);
}
//!!!!Получить авторизированного ползователя
int ACCESSMATRIX::getActiveSubject(void){
    return activeSubject;
}

//Просто дополнительный набор функций для вывода
class MATRIXOUT : public ACCESSMATRIX
{
public:
    MATRIXOUT(QString adminName, QString adminPassword) : ACCESSMATRIX(adminName, adminPassword){}
    ~MATRIXOUT(){}
    QStringList getSubjects();
    QStringList getObjects();
    int getObjectCount();
    int getSubjectCount();
    QString interpreteRules(int mySubjNum, int myObjNum);
};
//возвращает список имен субъектов (удобно для использования в  listWidget)
QStringList MATRIXOUT::getSubjects()
{
    QStringList list;
    for(int i = 0; i < subjects.size(); i++)
    {
        list << subjects[i].getName();
    }
    return list;
}
//возвращает список имен объектов (удобно для использования в  listWidget)
QStringList MATRIXOUT::getObjects()
{
    QStringList list;
    for(int i = 0; i < objects.size(); i++)
    {
        list << objects[i].getName();
    }
    return list;
}
int MATRIXOUT::getObjectCount()
{
    return objects.size();
}
int MATRIXOUT::getSubjectCount()
{
    return subjects.size();
}
//интерпретирует биты прав в удобный для чтения вид
QString MATRIXOUT::interpreteRules(int mySubjNum, int myObjNum)
{
    QString ruleString;
    if(canRead(mySubjNum, myObjNum) == true)
    {
        ruleString+="r ";
    }
    if(canWrite(mySubjNum, myObjNum) == true)
    {
        ruleString+="w ";
    }
    if(isOwner(mySubjNum, myObjNum) == true)
    {
        ruleString+="o";
    }
    return ruleString;
}

#endif // HRU


#ifndef BELLA
#define BELLA
#include <vector>
#include <string>
#include <QString>
//==========================================LEVEL
class LEVEL
{
public:
    LEVEL(unsigned int myLevelNum, std::string myLevelName):levelNum(myLevelNum), levelName(myLevelName){}
    LEVEL(){}
    ~LEVEL(){}
    bool operator == (LEVEL lvl1);
    LEVEL operator = (LEVEL lvl1);
    bool operator < (LEVEL lvl1);
    bool operator > (LEVEL lvl1);
    void Set_Level_Name(std::string myLevelName);
    std::string Get_Level_Name();
    void Set_Level_Num(unsigned int myLevelNum);
    unsigned int Get_Level_Num();
private:
    unsigned int levelNum;
    std::string levelName;
};
bool LEVEL::operator ==(LEVEL lvl1)
{
    if(this->levelNum == lvl1.levelNum)
        return true;
    return false;
}
LEVEL LEVEL::operator = (LEVEL lvl1)
{
    this->levelName = lvl1.levelName;
    this->levelNum = lvl1.levelNum;
    return *this;
}
bool LEVEL::operator < (LEVEL lvl1)
{
    if(this->levelNum < lvl1.levelNum)
        return true;
    return false;
}

bool LEVEL::operator > (LEVEL lvl1)
{
    if(this->levelNum > lvl1.levelNum)
        return true;
    return false;
}
void LEVEL::Set_Level_Name(std::string myLevelName)
{
    levelName = myLevelName;
}

std::string LEVEL::Get_Level_Name()
{
    return levelName;
}

void LEVEL::Set_Level_Num(unsigned int myLevelNum)
{
    levelNum = myLevelNum;
}

unsigned int LEVEL::Get_Level_Num()
{
    return levelNum;
}
//==========================================OBJECT
class OBJECT
{
public:
    OBJECT(){}
    ~OBJECT(){}
    void Set_Security_Level(LEVEL myLvl);
    LEVEL Get_Security_Level();
    void Set_Integrity_Level(LEVEL myLvl);
    LEVEL Get_Integrity_Level();
    void Set_Name(std::string myName);
    std::string Get_Name();
private:
    LEVEL objIntegrityLevel;
    LEVEL objSecurityLevel;
    std::string objName;
};

void OBJECT::Set_Integrity_Level(LEVEL myLvl)
{
    objIntegrityLevel = myLvl;
}

LEVEL OBJECT::Get_Integrity_Level()
{
    return objIntegrityLevel;
}

void OBJECT::Set_Security_Level(LEVEL myLvl)
{
    objSecurityLevel = myLvl;
}

LEVEL OBJECT::Get_Security_Level()
{
    return objSecurityLevel;
}

void OBJECT::Set_Name(std::string myName)
{
    objName = myName;
}

std::string OBJECT::Get_Name()
{
    return objName;
}

//==========================================SUBJECT
class SUBJECT
{
public:
    SUBJECT(){}
    ~SUBJECT(){}
    std::string Get_Name(void);
    void Set_Name(std::string myName);
    void Set_Password(std::string myPassword);
    bool Log_In(std::string myPassword);
    void Set_Security_Level(LEVEL myLvl);
    LEVEL Get_Security_Level();
    void Set_Integrity_Level(LEVEL myLvl);
    LEVEL Get_Integrity_Level();
private:
    std::string subjName;
    std::string subjPassword;
    LEVEL subjSecurityLevel;//уровень в моделе Белла-Лапвдулы
    LEVEL subjIntegrityLevel;//уровень в моделе Биба
};
void SUBJECT::Set_Integrity_Level(LEVEL myLvl)
{
    subjIntegrityLevel = myLvl;
}

LEVEL SUBJECT::Get_Integrity_Level()
{
    return subjIntegrityLevel;
}

void SUBJECT::Set_Security_Level(LEVEL myLvl)
{
    subjSecurityLevel = myLvl;
}

LEVEL SUBJECT::Get_Security_Level()
{
    return subjSecurityLevel;
}

bool SUBJECT::Log_In(std::string myPassword)
{
    if(myPassword == subjPassword)
        return true;
    return false;
}
std::string SUBJECT::Get_Name(void)
{
    return subjName;
}
void SUBJECT::Set_Name(std::string myName)
{
    subjName = myName;
}

void SUBJECT::Set_Password(std::string myPassword)
{
    subjPassword = myPassword;
}
//==========================================MODEL_BELLA
#define Nobody -1
#define Admin 1
class MODEL
{
public: 
    MODEL(std::string myName, std::string myPassword);
    MODEL(){}
    ~MODEL(){}
    int Add_Object(std::string objName, LEVEL intLevel,LEVEL secLevel);
    int Add_Subject(std::string subjName, std::string myPassword, LEVEL intLevel, LEVEL secLevel);
    int Dell_Object(std::string objName);
    int Dell_Subject(std::string subjName);
    int Read_Object(std::string objName);
    int Write_Object(std::string objName);
  //  int Change_Security_Level(LEVEL myLvl);
//    int Change_Integrity_Level(LEVEL myLvl);
    std::vector<OBJECT> Get_Objects_From_Level(LEVEL myLvl, int flag = 0);//flag == 0 - поданый уровень - уровень безопасности
    std::vector<SUBJECT> Get_Subjects_From_Level(LEVEL myLvl, int flag = 0);//flag == 1 - поданый в качестве 1 аргумента уровень - уровень целостности
    bool Log_In(std::string mySubjName, std::string myPassword);
protected:
    std::vector<SUBJECT> subjects;
    std::vector<OBJECT> objects;
    std::vector<LEVEL> securityLevels;
    std::vector<LEVEL> integrityLevels;
    int activeSubject;
    int activeSecurityLevel;
    int activeIntegrityLevel;
};
MODEL::MODEL(std::string myName, std::string myPassword)
{
    activeSecurityLevel = Nobody;
    activeIntegrityLevel = Nobody;
    activeSubject = Nobody;
    objects.resize(objects.size() + 1);
    subjects.resize(subjects.size() + 1);
    objects[0].Set_Name("Access grid");//Решетка доступа
    subjects[0].Set_Name(myName);
    subjects[0].Set_Password(myPassword);
    securityLevels.resize(securityLevels.size() + 1);
    integrityLevels.resize(integrityLevels.size() + 1);
    securityLevels[0].Set_Level_Num(Admin);
    integrityLevels[0].Set_Level_Num(Admin);
    securityLevels[0].Set_Level_Name("Absolutely Secret");
    integrityLevels[0].Set_Level_Name("Untouched");//Целостный файл
    subjects[0].Set_Integrity_Level(integrityLevels[0]);
    subjects[0].Set_Security_Level(securityLevels[0]);
    objects[0].Set_Integrity_Level(integrityLevels[0]);
    objects[0].Set_Security_Level(securityLevels[0]);
}

int MODEL::Add_Object(std::string objName, LEVEL intLevel,LEVEL secLevel)
{
    if(activeSecurityLevel == Nobody)
        return -1;
    for(unsigned int i = 0; i < objects.size(); ++i)
    {
        if(objects[i].Get_Name() == objName)
            return -1;
    }
    if(subjects[activeSecurityLevel].Get_Security_Level() > secLevel)//NoWriteDown
        return -1;
    objects.resize(objects.size() + 1);
    objects[objects.size() - 1].Set_Name(objName);
    objects[objects.size() - 1].Set_Security_Level(secLevel);
    objects[objects.size() - 1].Set_Integrity_Level(intLevel);
    return 0;
}
//добавляет субъекты админ
int MODEL::Add_Subject(std::string subjName, std::string myPassword, LEVEL intLevel, LEVEL secLevel)
{
    bool lvlExistFlag = false;
    if(activeSecurityLevel != Admin)
        return -1;
    for(unsigned int i = 0; i < subjects.size(); ++i)
    {
        if(subjects[i].Get_Name() == subjName)
            return -1;
    }
    for(unsigned int j = 0; j < securityLevels.size(); ++j)
    {
        if(secLevel == securityLevels[j])
        {
            lvlExistFlag = true;
            break;
        }
    }
    if(lvlExistFlag == false)
    {
        securityLevels.resize(securityLevels.size() + 1);
        securityLevels[securityLevels.size() - 1] = secLevel;
    }

    lvlExistFlag = false;

    for(unsigned int k = 0; k < integrityLevels.size(); ++k)
    {
        if(intLevel == integrityLevels[k])
        {
            lvlExistFlag = true;
            break;
        }
    }
    if(lvlExistFlag == false)
    {
        integrityLevels.resize(integrityLevels.size() + 1);
        integrityLevels[integrityLevels.size() - 1] = intLevel;
    }

    subjects.resize(subjects.size() + 1);
    subjects[subjects.size() - 1].Set_Name(subjName);
    subjects[subjects.size() - 1].Set_Password(myPassword);
    subjects[subjects.size() - 1].Set_Security_Level(secLevel);
    subjects[subjects.size() - 1].Set_Integrity_Level(intLevel);
    return 0;
}
//удалить мжно самого себя, или кого угодно с верхнего уровня. Удалить пользователя номер 1 нельзя.
int MODEL::Dell_Subject(std::string subjName)//добавить проверку на последнего пацана в группе.
{
    unsigned int subjNum = 0;
    if((subjects[activeSubject - 1].Get_Name() == subjName) || activeSecurityLevel == Admin)
    {
        for(unsigned int i = 0; i < subjects.size(); ++i)
        {
            if(subjects[i].Get_Name() == subjName)
            {
                subjNum = i;
                break;
            }
        }
        if(subjNum == 0)
            return -1;
        std::vector<SUBJECT>::iterator it;
        it = subjects.begin() + subjNum;
        subjects.erase(it, it + 1);
        return 0;
    }
    return -1;
}
//Удалять объекты могуть только субъекты того-же уровня доступа, что и объект
int MODEL::Dell_Object(std::string objName)
{
    unsigned int objNum = 0;
    if(activeSubject == Nobody)
        return -1;
    for(unsigned int i = 0; i < objects.size(); ++i)
    {
        if(objects[i].Get_Name() == objName)
        {
            objNum = i;
            break;
        }
    }
    if(objNum == 0)
        return -1;
    std::vector<OBJECT>::iterator it;
    if((subjects[activeSubject - 1].Get_Security_Level() == objects[objNum].Get_Security_Level()) || (activeSecurityLevel == Admin))
    {
        it = objects.begin() + objNum;
        objects.erase(it, it + 1);
        return 0;
    }
    return -1;
}
//реально ничего не читает. просто возвращает 0, если возможно прочесть данный объект с субъекта
int MODEL::Read_Object(std::string objName)
{
    unsigned int objNum = 0;
    if(activeSubject == Nobody)
        return -1;
    for(unsigned int i = 0; i < objects.size(); ++i)
    {
        if(objects[i].Get_Name() == objName)
        {
            objNum = i;
            break;
        }
    }
    if(objNum == 0)
        return -1;
    if(subjects[activeSubject - 1].Get_Security_Level() < objects[objNum].Get_Security_Level())//попытка читать уровни выше данного
        return -1;
    //получение доступа
    if(subjects[activeSubject - 1].Get_Integrity_Level() < objects[objNum].Get_Integrity_Level())//для модели Биба. Если читаем ниже, то переходим на уровень ниже.
    {
        subjects[activeSubject - 1].Set_Integrity_Level(objects[objNum].Get_Integrity_Level());
    }
    return 0;
}

int MODEL::Write_Object(std::string objName)
{
    unsigned int objNum = 0;
    if(activeSubject == Nobody)
        return -1;
    for(unsigned int i = 0; i < objects.size(); ++i)
    {
        if(objects[i].Get_Name() == objName)
        {
            objNum = i;
            break;
        }
    }
    if(objNum == 0)
        return -1;
    if(subjects[activeSubject - 1].Get_Security_Level() > objects[objNum].Get_Security_Level())//попытка писать в уровни ниже данного
        return -1;
    if(subjects[activeSubject - 1].Get_Integrity_Level() < objects[objNum].Get_Integrity_Level())//для модели Биба. Если пишем выше, то объект переходит на уровень ниже.
    {
        objects[objNum].Set_Integrity_Level(subjects[activeSubject - 1].Get_Integrity_Level());
    }
    return 0;
}

bool MODEL::Log_In(std::string mySubjName, std::string myPassword)
{
    int subjNum = -1;
    for(unsigned int i = 0; i < subjects.size(); ++i)
    {
        if(subjects[i].Get_Name() == mySubjName)
        {
            subjNum = i;
        }
    }
    if(subjNum == -1)
        return false;
    if(subjects[subjNum].Log_In(myPassword) == true)
    {
        activeSubject = subjNum + 1;
        activeIntegrityLevel = subjects[subjNum].Get_Integrity_Level().Get_Level_Num();
        activeSecurityLevel = subjects[subjNum].Get_Security_Level().Get_Level_Num();
        return true;
    }
    return false;
}

std::vector<OBJECT> MODEL::Get_Objects_From_Level(LEVEL myLvl, int flag)
{
    std::vector<OBJECT> retVec;
    if(flag == 0)
    {
        for(unsigned int i = 0; i < objects.size(); ++i)
        {
            if(objects[i].Get_Security_Level() == myLvl)
            {
                retVec.resize(retVec.size() + 1);
                retVec[retVec.size() - 1] = objects[i];
            }
        }
    }
    if(flag == 1)
    {
        for(unsigned int i = 0; i < objects.size(); ++i)
        {
            if(objects[i].Get_Integrity_Level() == myLvl)
            {
                retVec.resize(retVec.size() + 1);
                retVec[retVec.size() - 1] = objects[i];
            }
        }
    }
    return retVec;
}

std::vector<SUBJECT> MODEL::Get_Subjects_From_Level(LEVEL myLvl, int flag)
{
    std::vector<SUBJECT> retVec;
    if(flag == 0)
    {
        for(unsigned int i = 0; i < subjects.size(); ++i)
        {
            if(subjects[i].Get_Security_Level() == myLvl)
            {
                retVec.resize(retVec.size() + 1);
                retVec[retVec.size() - 1] = subjects[i];
            }
        }
    }
    if(flag == 1)
    {
        for(unsigned int i = 0; i < subjects.size(); ++i)
        {
            if(subjects[i].Get_Integrity_Level() == myLvl)
            {
                retVec.resize(retVec.size() + 1);
                retVec[retVec.size() - 1] = subjects[i];
            }
        }
    }
    return retVec;
}
class MODELOUT: public MODEL{
public:
    MODELOUT(std::string myName, std::string myPassword):MODEL(myName, myPassword){}
    ~MODELOUT(){}
    QStringList Get_Subjects();
    QStringList Get_Objects();
    unsigned int Get_Count_Subjects();
    unsigned int Get_Count_Objects();
};

QStringList MODELOUT::Get_Subjects(){
    QStringList list;
    QString subject;
    for(unsigned int i = 0; i < subjects.size(); i++)
    {
        subject = subject.fromStdString(subjects[i].Get_Name());
        list << subject;
    }
    return list;
}
QStringList MODELOUT::Get_Objects(){
    QStringList list;
    QString object;
    for(unsigned int i = 0; i < objects.size(); i++)
    {
      object = object.fromStdString(objects[i].Get_Name());
      list << object;
    }
    return list;
}
unsigned int MODELOUT::Get_Count_Subjects(){
    return subjects.size();
}
unsigned int MODELOUT::Get_Count_Objects(){
    return objects.size();
}
#endif // BELLA


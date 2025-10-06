#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

enum class UserType {Manager, Client};
enum class Status {Accepted, Progres, Ready, Issued};

class User{
private:
    QString name;
    UserType role;
public:
    User(QString name, UserType role) : name(name), role(role) {}
    QString getName() const {return name;}
    UserType getRole() const {return role;}
};
class Device{
private:
    QString type;
public:
    Device(QString type) : type(type) {}
    QString getType() const {return type;}
};

class Part{
private:
    QString name;
    int count;
public:
    Part(QString name, int count) : name(name), count(count) {}
    QString getName() const {return name;}
    int getCount() const {return count;}

    void use(int cout){
        if(cout <= count){
            count -= cout;
        }
    }
};

class StatusTracker {
private:
    QVector<QString> statusHistory;
public:
    StatusTracker() { statusHistory.push_back("Создан"); }
    void addStatus(const QString& status) {
        statusHistory.push_back(status);
    }
    QVector<QString> getHistory() const { return statusHistory; }
};

class RepairOrder{
private:
    int id;
    User client;
    Device device;
    QString problemClient;
    StatusTracker tracker;
    Status currentStatus;
    //Замеена статуса
    bool changeStatus(Status newStatus){
        switch (currentStatus){
            case Status::Accepted: return newStatus == Status::Progres;
            case Status::Progres:  return newStatus == Status::Ready;
            case Status::Ready:    return newStatus == Status::Issued;
            case Status::Issued:   return false;
        }
        return false;
    }
public:
    RepairOrder(int id, User& client, Device& device, QString& problemClient, StatusTracker tracker) : id(id), client(client), device(device), problemClient(problemClient), tracker(tracker), currentStatus(Status::Accepted) {}

    int getId() const {return id;}
    User getUser() const {return client;}
    Device getDevice() const {return device;}
    QString getProblem() const {return problemClient;}
    Status getStatus() const {return currentStatus;}

    void changeStatus(Status newStatus){
        if(ChangeStatus(newStatus)){
            currentStatus == newStatus;

        }
    }


};

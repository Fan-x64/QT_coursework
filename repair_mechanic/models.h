#ifndef MODELS_H
#define MODELS_H

#include <QString>
#include <QVector>
#include <QDateTime>
#include <QDebug>
#include <QTextStream>
#include <QDate>
#include <QFile>
#include <string>
//________________________________________


enum class UserType { Manager, Client };
enum class Status { Pending, Accepted, Progres, Ready, Issued };

class User {
private:
    QString name;
    UserType role;

public:
    User(const QString& name, UserType role)
        : name(name), role(role) {}

    QString getName() const { return name; }
    UserType getRole() const { return role; }
};

class Device {
private:
    QString type;

public:
    Device(const QString& type) : type(type) {}

    QString getType() const { return type; }
};

class Part {
private:
    QString name;
    int count;

public:
    Part(const QString& name, int count) : name(name), count(count) {}

    QString getName() const { return name; }
    int getCount() const { return count; }

    void use(int amount) {
        if (amount <= count) {
            count -= amount;
        }
    }
};

class StatusTracker {
private:
    QVector<QString> statusHistory;

public:
    StatusTracker() {
        statusHistory.push_back("Создан");
    }

    void addStatus(const QString& status) {
        statusHistory.push_back(status);
    }

    QVector<QString> getHistory() const { return statusHistory; }
};

class RepairOrder {
private:
    int id;
    User client;
    Device device;
    QString problemClient;
    StatusTracker tracker;
    Status currentStatus;
    QVector<Part> usedParts;

    bool canChangeTo(Status newStatus) const {
        switch (currentStatus) {
        case Status::Pending:  return newStatus == Status::Accepted;
        case Status::Accepted: return newStatus == Status::Progres;
        case Status::Progres:  return newStatus == Status::Ready;
        case Status::Ready:    return newStatus == Status::Issued;
        case Status::Issued:   return false;
        }
        return false;
    }

public:
    RepairOrder(int id, const User& client, const Device& device, const QString& problemClient)
        : id(id), client(client), device(device), problemClient(problemClient),
        tracker(), currentStatus(Status::Pending), usedParts() {
        tracker.addStatus("Pending");
    }

    int getId() const                  { return id; }
    User getClient() const             { return client; }
    Device getDevice() const           { return device; }
    QString getProblem() const         { return problemClient; }
    Status getStatus() const           { return currentStatus; }
    QVector<Part> getUsedParts() const { return usedParts; }

    void changeStatus(Status newStatus) {
        if (canChangeTo(newStatus)) {
            currentStatus = newStatus;
            tracker.addStatus(QString::fromStdString(statusToString(newStatus)));
        }
    }

    void addPart(const Part& part, int count) {
        if (currentStatus == Status::Progres) {
            Part used(part.getName(), count);
            usedParts.push_back(used);
        }
    }

    QString getAction() const {
        switch (currentStatus) {
        case Status::Pending:  return "В ожидании";
        case Status::Accepted: return "Принят в работу, ожидайте";
        case Status::Progres:  return "В работе, ожидайте окончания работы";
        case Status::Ready:    return "Заказ готов, подойдите к рабочему месту";
        case Status::Issued:   return "Заказ готов, оцените качество работы";
        }
        return "";
    }

    static std::string statusToString(Status s) {
        switch (s) {
        case Status::Pending:  return "Pending";
        case Status::Accepted: return "Accepted";
        case Status::Progres:  return "Progres";
        case Status::Ready:    return "Ready";
        case Status::Issued:   return "Issued";
        }
        return "";
    }
};

class OrderDatabase {
private:
    static QVector<RepairOrder*> orders;
    static int nextId;
    static QVector<Part> stockParts;

public:
    static void addOrder(RepairOrder* order) { orders.push_back(order); }
    static QVector<RepairOrder*> getAllOrders() { return orders; }
    static int getNextId() { return nextId++; }

    //gjbcr
    static QVector<RepairOrder*> searchByClient(const QString& clientName) {
        QVector<RepairOrder*> result;
        for (auto order : orders) {
            if (order->getClient().getName().contains(clientName, Qt::CaseInsensitive)) {
                result.push_back(order);
            }
        }
        return result;
    }

    //поиск по статусу
    static QVector<RepairOrder*> searchByStatus(Status status) {
        QVector<RepairOrder*> result;
        for (auto order : orders) {
            if (order->getStatus() == status) {
                result.push_back(order);
            }
        }
        return result;
    }

    // экспорт истории
    static void exportHistory(const QString& filePath) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            for (auto order : orders) {
                out << "ID: " << order->getId()
                << ", Client: " << order->getClient().getName()
                << ", Device: " << order->getDevice().getType()
                << ", Status: " << QString::fromStdString(
                       RepairOrder::statusToString(order->getStatus()))
                << "\n";
            }
            file.close();
        }
    }

    static void addStockPart(const Part& part) { stockParts.push_back(part); }
    static QVector<Part> getStock() { return stockParts; }
    static void usePartFromStock(const QString& partName, int amount) {
        for (auto& part : stockParts) {
            if (part.getName() == partName) {
                part.use(amount);
                break;
            }
        }
    }
};

QVector<RepairOrder*> OrderDatabase::orders;
int OrderDatabase::nextId = 1;
QVector<Part> OrderDatabase::stockParts;

#endif // MODELS_H

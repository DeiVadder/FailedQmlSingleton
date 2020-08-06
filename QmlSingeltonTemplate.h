#ifndef QMLSINGELTONTEMPLATE_H
#define QMLSINGELTONTEMPLATE_H

#include <QObject>
#include <QQmlEngine>

template <class T>
struct BaseObjectPointer {
    static QObject *instancePointer;
};

template <typename T>
QObject *BaseObjectPointer<T>::instancePointer = nullptr;
template <class T>
class QmlSingleton : public QObject, protected BaseObjectPointer<T>
{
protected:
    QmlSingleton(QObject *parent = nullptr) : QObject(parent){ }

    QmlSingleton(QmlSingleton const &) = delete ;
    QmlSingleton& operator=(QmlSingleton const &) = delete;

private:
    static inline void setCppOwnerShip(QQmlEngine *engine){
        QMetaObject::invokeMethod(BaseObjectPointer<T>::instancePointer, [=]()->void{
            engine->setObjectOwnership(BaseObjectPointer<T>::instancePointer, QQmlEngine::CppOwnership);
        }, Qt::QueuedConnection);
    }

public:
    static T *getInstance() {
        if(BaseObjectPointer<T>::instancePointer)
            return static_cast<T*>(BaseObjectPointer<T>::instancePointer);

        auto instance = new QmlSingleton(nullptr);
        BaseObjectPointer<T>::instancePointer = instance;
        return static_cast<T*>(instance);
    }

    static QObject *instance (QQmlEngine *engine, QJSEngine *){
        if(!BaseObjectPointer<T>::instancePointer)
            BaseObjectPointer<T>::instancePointer = new QmlSingleton(nullptr);

        setCppOwnerShip(engine);
        return BaseObjectPointer<T>::instancePointer;
    }

    static void resolve(){
        if(BaseObjectPointer<T>::instancePointer){
            BaseObjectPointer<T>::instancePointer->deleteLater();
            BaseObjectPointer<T>::instancePointer = nullptr;
        }
    }
};

#endif // QMLSINGELTONTEMPLATE_H

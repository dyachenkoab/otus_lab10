#ifndef IOBSERVER_H
#define IOBSERVER_H

enum class ObserverType {
    CONSOLE_ = 0,
    FILE_,
    MAX_TYPES
};

class Observer
{
public:
    virtual ~Observer() {};
    virtual void update() = 0;
    ObserverType type() const;

protected:
    Observer(ObserverType type);
    ObserverType m_type;
};

#endif // IOBSERVER_H

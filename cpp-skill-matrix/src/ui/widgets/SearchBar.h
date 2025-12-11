#ifndef SEARCHBAR_H
#define SEARCHBAR_H

#include <QWidget>

class SearchBar : public QWidget
{
    Q_OBJECT

public:
    explicit SearchBar(QWidget* parent = nullptr);
    ~SearchBar();
};

#endif // SEARCHBAR_H

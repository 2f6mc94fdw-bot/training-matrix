#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QWidget>

class TreeView : public QWidget
{
    Q_OBJECT

public:
    explicit TreeView(QWidget* parent = nullptr);
    ~TreeView();
};

#endif // TREEVIEW_H

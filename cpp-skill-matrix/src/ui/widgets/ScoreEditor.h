#ifndef SCOREEDITOR_H
#define SCOREEDITOR_H

#include <QWidget>

class ScoreEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ScoreEditor(QWidget* parent = nullptr);
    ~ScoreEditor();
};

#endif // SCOREEDITOR_H

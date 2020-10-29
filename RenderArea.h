#pragma once

#include <QtWidgets/QWidget>

class RenderArea : public QWidget
{
	Q_OBJECT

public:
	RenderArea(QWidget* parent = nullptr);

	QSize sizeHint() const override
	{
		return QSize(400, 300);
	}
	QSize minimumSizeHint() const override
	{
		return sizeHint();
	}

protected:
	void paintEvent(QPaintEvent* e) override;
};


/*
 * Copyright (c) 2021 Alex Spataru <https://github.com/alex-spataru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "WidgetLoader.h"

#include "Bar.h"
#include "Plot.h"
#include "Gauge.h"
#include "Compass.h"
#include "DataGroup.h"
#include "Gyroscope.h"
#include "Thermometer.h"

#include <QPushButton>
#include <QApplication>
#include <UI/Dashboard.h>
#include <Misc/ThemeManager.h>

using namespace Widgets;

/**
 * Constructor function
 */
WidgetLoader::WidgetLoader(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_index(-1)
    , m_widget(nullptr)
    , m_widgetVisible(false)
{
    // Set item flags
    setFlag(ItemHasContents, true);
    setFlag(ItemIsFocusScope, true);
    setFlag(ItemAcceptsInputMethod, true);
    setAcceptedMouseButtons(Qt::AllButtons);

    // Configure main window
    m_window.setMinimumWidth(640);
    m_window.setMinimumHeight(480);

    // Configure window style sheet
    auto theme = Misc::ThemeManager::getInstance();
    auto qss
        = QString("background-color: %1;").arg(theme->datasetWindowBackground().name());
    m_window.setStyleSheet(qss);

    // Resize widget to fit QML item size
    connect(this, &QQuickPaintedItem::widthChanged, this,
            &WidgetLoader::updateWidgetSize);
    connect(this, &QQuickPaintedItem::heightChanged, this,
            &WidgetLoader::updateWidgetSize);

    // Automatically update the widget's visibility
    connect(UI::Dashboard::getInstance(), &UI::Dashboard::widgetVisibilityChanged, this,
            &WidgetLoader::updateWidgetVisible);

    // Enable/disable the window widget automatically
    connect(&m_window, SIGNAL(visibleChanged()), this, SLOT(updateWidgetWindow()));
}

/**
 * Delete widget on class destruction
 */
WidgetLoader::~WidgetLoader()
{
    if (m_widget)
        delete m_widget;
}

/**
 * Handle application events manually
 */
bool WidgetLoader::event(QEvent *event)
{
    if (!m_widget)
        return false;

    switch (event->type())
    {
        case QEvent::FocusIn:
            forceActiveFocus();
            return QQuickPaintedItem::event(event);
            break;
        case QEvent::Wheel:
            processWheelEvents(static_cast<QWheelEvent *>(event));
            return true;
            break;
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseMove:
            processMouseEvents(static_cast<QMouseEvent *>(event));
            return true;
            break;
        default:
            break;
    }

    return QApplication::sendEvent(m_widget, event);
}

/**
 * Render the widget on the given @a painter
 */
void WidgetLoader::paint(QPainter *painter)
{
    if (m_widget && painter)
        m_widget->render(painter);
}

/**
 * Custom event filter to manage redraw requests
 */
bool WidgetLoader::eventFilter(QObject *watched, QEvent *event)
{
    if (!m_widget)
        return false;

    if (watched == m_widget)
    {
        switch (event->type())
        {
            case QEvent::Paint:
            case QEvent::UpdateRequest:
                update();
                break;
            default:
                break;
        }
    }

    return QQuickPaintedItem::eventFilter(watched, event);
}

/**
 * Returns the global index of the widget (index of the current widget in relation to all
 * registered widgets).
 */
int WidgetLoader::widgetIndex() const
{
    return m_index;
}

/**
 * Returns the relative index of the widget (e.g. index of a bar widget in relation to the
 * total number of bar widgets).
 */
int WidgetLoader::relativeIndex() const
{
    return UI::Dashboard::getInstance()->relativeIndex(widgetIndex());
}

/**
 * Returns @c true if the QML interface should display this widget.
 */
bool WidgetLoader::widgetVisible() const
{
    return m_widgetVisible;
}

/**
 * Returns the path of the SVG icon to use with this widget
 */
QString WidgetLoader::widgetIcon() const
{
    return UI::Dashboard::getInstance()->widgetIcon(widgetIndex());
}

/**
 * Returns the appropiate window title for the given widget
 */
QString WidgetLoader::widgetTitle() const
{
    return UI::Dashboard::getInstance()->widgetTitles().at(widgetIndex());
}

/**
 * Returns the type of the current widget (e.g. group, plot, bar, gauge, etc...)
 */
UI::Dashboard::WidgetType WidgetLoader::widgetType() const
{
    return UI::Dashboard::getInstance()->widgetType(widgetIndex());
}

/**
 * Shows a window with the current widget
 */
void WidgetLoader::displayWindow()
{
    m_window.showNormal();
}

/**
 * Selects & configures the appropiate widget for the given @a index
 */
void WidgetLoader::setWidgetIndex(const int index)
{
    if (index < UI::Dashboard::getInstance()->totalWidgetCount() && index >= 0)
    {
        // Update widget index
        m_index = index;

        // Delete previous widget
        if (m_widget)
        {
            delete m_widget;
            m_widget = nullptr;
        }

        // Delete central widget of window
        if (m_window.centralWidget())
            delete m_window.centralWidget();

        // Hide widget window
        if (m_window.isVisible())
            m_window.hide();

        // Construct new widget
        switch (widgetType())
        {
            case UI::Dashboard::WidgetType::Group:
                m_widget = new DataGroup(relativeIndex());
                m_window.setCentralWidget(new DataGroup(relativeIndex()));
                break;
            case UI::Dashboard::WidgetType::Plot:
                m_widget = new QPushButton("Plot");
                break;
            case UI::Dashboard::WidgetType::Bar:
                m_widget = new QPushButton("Bar");
                break;
            case UI::Dashboard::WidgetType::Gauge:
                m_widget = new QPushButton("Gauge");
                break;
            case UI::Dashboard::WidgetType::Thermometer:
                m_widget = new QPushButton("Thermometer");
                break;
            case UI::Dashboard::WidgetType::Compass:
                m_widget = new Compass(relativeIndex());
                m_window.setCentralWidget(new Compass(relativeIndex()));
                break;
            case UI::Dashboard::WidgetType::Gyroscope:
                m_widget = new QPushButton("Gyroscope");
                break;
            case UI::Dashboard::WidgetType::Accelerometer:
                m_widget = new QPushButton("Accelerometer");
                break;
            case UI::Dashboard::WidgetType::Map:
                m_widget = new QPushButton("Map");
                break;
            default:
                break;
        }

        // Update window title
        m_window.setWindowTitle(widgetTitle());
        if (m_window.centralWidget())
            m_window.centralWidget()->setEnabled(false);

        // Allow widget to receive events from the QML interface
        if (m_widget)
        {
            m_widget->installEventFilter(this);
            emit widgetIndexChanged();
            updateWidgetVisible();
        }
    }
}

/**
 * Resizes the widget to fit inside the QML item.
 */
void WidgetLoader::updateWidgetSize()
{
    if (m_widget && width() > 0 && height() > 0)
    {
        m_widget->setFixedSize(width(), height());
        update();
    }
}

/**
 * Enables/disables the widget updates of the external window when the window
 * is shown or hidden.
 */
void WidgetLoader::updateWidgetWindow()
{
    if (m_window.centralWidget())
        m_window.centralWidget()->setEnabled(m_window.isVisible());
}

/**
 * Updates the visibility status of the current widget (this function is called
 * automatically by the UI::Dashboard class via signals/slots).
 */
void WidgetLoader::updateWidgetVisible()
{
    bool visible = UI::Dashboard::getInstance()->widgetVisible(widgetIndex());

    if (widgetVisible() != visible)
    {
        m_widgetVisible = visible;
        emit widgetVisibleChanged();
    }
}

/**
 * Hack: calls the appropiate protected mouse event handler function of the widget item
 */
void WidgetLoader::processMouseEvents(QMouseEvent *event)
{
    if (!m_widget)
        return;

    class Hack : public QWidget
    {
    public:
        using QWidget::mouseDoubleClickEvent;
        using QWidget::mouseMoveEvent;
        using QWidget::mousePressEvent;
        using QWidget::mouseReleaseEvent;
    };

    auto hack = static_cast<Hack *>(m_widget);
    switch (event->type())
    {
        case QEvent::MouseButtonPress:
            hack->mousePressEvent(event);
            break;
        case QEvent::MouseMove:
            hack->mouseMoveEvent(event);
            break;
        case QEvent::MouseButtonRelease:
            hack->mouseReleaseEvent(event);
            break;
        case QEvent::MouseButtonDblClick:
            hack->mouseDoubleClickEvent(event);
            break;
        default:
            break;
    }
}

/**
 * Hack: calls the appropiate protected wheel event handler function of the widget item
 */
void WidgetLoader::processWheelEvents(QWheelEvent *event)
{
    if (!m_widget)
        return;

    class Hack : public QWidget
    {
    public:
        using QWidget::wheelEvent;
    };

    static_cast<Hack *>(m_widget)->wheelEvent(event);
}
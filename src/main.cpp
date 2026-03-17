#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("OncoFlow Monitor");
    app.setOrganizationName("OncoFlow");
    app.setApplicationVersion("1.0.0");

    // Professional medical software styling
    app.setStyleSheet(R"(
        QMainWindow {
            background-color: #f5f5f5;
        }
        QGroupBox {
            font-weight: bold;
            border: 1px solid #c0c0c0;
            border-radius: 4px;
            margin-top: 8px;
            padding-top: 16px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 4px;
        }
        QToolBar {
            spacing: 6px;
            padding: 4px;
            border-bottom: 1px solid #c0c0c0;
        }
        QPushButton {
            padding: 4px 12px;
            border: 1px solid #b0b0b0;
            border-radius: 3px;
            background-color: #ffffff;
        }
        QPushButton:hover {
            background-color: #e8e8e8;
        }
        QPushButton:pressed {
            background-color: #d0d0d0;
        }
        QPushButton:disabled {
            color: #a0a0a0;
            background-color: #f0f0f0;
        }
        QTreeWidget {
            border: 1px solid #c0c0c0;
            border-radius: 2px;
        }
        QTreeWidget::item {
            padding: 2px 0;
        }
        QProgressBar {
            border: 1px solid #c0c0c0;
            border-radius: 3px;
            text-align: center;
            height: 18px;
        }
        QProgressBar::chunk {
            background-color: #0078d7;
            border-radius: 2px;
        }
        QLineEdit, QComboBox, QDoubleSpinBox {
            padding: 3px 6px;
            border: 1px solid #c0c0c0;
            border-radius: 3px;
        }
        QTabWidget::pane {
            border: 1px solid #c0c0c0;
        }
        QStatusBar {
            border-top: 1px solid #c0c0c0;
        }
    )");

    MainWindow window;
    window.show();

    return app.exec();
}

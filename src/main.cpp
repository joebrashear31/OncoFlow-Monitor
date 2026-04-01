#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("OncoFlow Monitor");
    app.setOrganizationName("OncoFlow");
    app.setApplicationVersion("1.0.0");

    // Palette-aware styling — works with both light and dark system themes
    app.setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            border: 1px solid palette(mid);
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
            border-bottom: 1px solid palette(mid);
        }
        QPushButton {
            padding: 4px 12px;
            border: 1px solid palette(mid);
            border-radius: 3px;
        }
        QTreeWidget {
            border: 1px solid palette(mid);
            border-radius: 2px;
        }
        QTreeWidget::item {
            padding: 2px 0;
        }
        QProgressBar {
            border: 1px solid palette(mid);
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
            border: 1px solid palette(mid);
            border-radius: 3px;
        }
        QTabWidget::pane {
            border: 1px solid palette(mid);
        }
        QStatusBar {
            border-top: 1px solid palette(mid);
        }
    )");

    MainWindow window;
    window.show();

    return app.exec();
}

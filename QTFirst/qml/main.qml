import QtQuick 2.14
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14
import QtQuick.Layouts 1.3
import QtQuick.Window 2.14


Window {
    id: root
    width: 1200
    height: 800
    visible: true
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.Window
    property bool isDragging: false


    // 记录普通状态下的位置和大小（用于还原）
    property bool  isZoomed: false
    property real  normalX:   100
    property real  normalY:   80
    property real  normalW:   1200
    property real  normalH:   800

    // 给几何属性加平滑动画（苹果风关键）
    Behavior on x {
        enabled: !root.isDragging
        NumberAnimation { duration: 220; easing.type: Easing.InOutCubic }
    }
    Behavior on y {
        enabled: !root.isDragging
        NumberAnimation { duration: 220; easing.type: Easing.InOutCubic }
    }
    Behavior on width  { NumberAnimation { duration: 220; easing.type: Easing.InOutCubic } }
    Behavior on height { NumberAnimation { duration: 220; easing.type: Easing.InOutCubic } }


    Rectangle {
        id: mainFrame
        anchors.fill: parent
        width: 600
        height: 800
        radius: 16
        color: "#147712"
        clip:true
        ColumnLayout 
        {
            anchors.fill: parent
            anchors.margins: 1
            spacing: 0

            // 顶部栏（简单版，你后面可以换成自己单独的 TitleBar 组件）
            Rectangle 
            {
                id: topBar
                Layout.fillWidth: true
                Layout.preferredHeight: 48
                color: "#181818"

                RowLayout  {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 8

                    // 左边：头像 / 图标
                    Rectangle {
                        width: 32
                        height: 32
                        radius: 16
                        color: "#ffaa00"
                        Layout.alignment: Qt.AlignVCenter
                    }

                    // 搜索框占位
                    Rectangle {
                        Layout.alignment: Qt.AlignVCenter
                        width: 260
                        height: 30
                        radius: 4
                        color: "#222222"

                        Text {
                            anchors.centerIn: parent
                            text: "搜索股票 / 代码"
                            color: "#666666"
                            font.pixelSize: 12
                        }
                    }
                    // 中间空白
                    Item 
                    {
                        id: dragArea                  // ← 必须有这个
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                                        // 拖拽移动窗口
                        MouseArea 
                        {
                            anchors.fill: parent

                            property real pressX
                            property real pressY

                            onPressed: {
                                if (mouse.button === Qt.LeftButton) {
                                    root.isDragging = true
                                    pressX = mouse.x
                                    pressY = mouse.y
                                }
                            }
                            onReleased: {
                                root.isDragging = false
                            }

                            onCanceled: {
                                root.isDragging = false
                            }
                            
                            onPositionChanged: 
                            {
                                if (mouse.buttons & Qt.LeftButton)                   
                                {
                                    // 相对移动量
                                    var dx = mouse.x - pressX
                                    var dy = mouse.y - pressY

                                    root.x += dx
                                    root.y += dy
                                }
                            }
                        }
                        
                    }

                    // 右侧窗口控制按钮（简单版）
                    Row 
                    {
                        spacing: 6
                        Layout.alignment: Qt.AlignVCenter

                        ToolButton {
                            text: "—"
                            onClicked: root.showMinimized()
                        }

                        ToolButton {
                            text: "□"
                            onClicked: {
                                if (!root.isZoomed) {
                                    // 记录当前正常窗口的几何，用于还原
                                    root.normalX = root.x
                                    root.normalY = root.y
                                    root.normalW = root.width
                                    root.normalH = root.height

                                    root.isZoomed = true

                                    // 目标：占满当前屏幕工作区（留一点点边距也可以）
                                    // 简单写法：直接全屏
                                    root.x = Screen.width  > 0 ? 0 : root.x
                                    root.y = Screen.height > 0 ? 0 : root.y
                                    root.width  = Screen.width
                                    root.height = Screen.height
                                } else {
                                    // 还原到之前记录的位置和大小
                                    root.isZoomed = false
                                    root.x = root.normalX
                                    root.y = root.normalY
                                    root.width  = root.normalW
                                    root.height = root.normalH
                                }
                            }
                        }
                        ToolButton {
                            text: "×"
                            onClicked: root.close()
                        }
                    }
                }

            }

            // 中间三列区域
            SplitView {
                id: splitView
                Layout.fillWidth: true
                Layout.fillHeight: true
                orientation: Qt.Horizontal

                // 左侧：导航栏
                Rectangle {
                    id: leftNav
                    color: "#171717"
                    SplitView.preferredWidth: 72
                    SplitView.minimumWidth: 60

                    Column {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 16
                        spacing: 16

                        // 几个占位图标按钮
                        Repeater {
                            model: 6
                            delegate: Rectangle {
                                width: 36
                                height: 36
                                radius: 18
                                color: index === 0 ? "#2d8cf0" : "#252525"

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        // TODO: 切换不同自选分组 / 市场
                                    }
                                }
                            }
                        }
                    }
                }

                // 中间：股票列表
                Rectangle {
                    id: middleList
                    color: "#141414"
                    SplitView.preferredWidth: 320
                    SplitView.minimumWidth: 260

                    ListView {
                        id: stockList
                        anchors.fill: parent
                        anchors.margins: 4
                        clip: true
                        spacing: 8

                        model: ListModel {
                            ListElement { symbol: "600000"; name: "浦发银行"; last: 9.88; change: 0.12 }
                            ListElement { symbol: "600519"; name: "贵州茅台"; last: 1650.00; change: -12.34 }
                            ListElement { symbol: "000001"; name: "平安银行"; last: 11.23; change: 0.05 }
                            ListElement { symbol: "300750"; name: "宁德时代"; last: 180.45; change: -1.23 }
                            ListElement { symbol: "510300"; name: "沪深300ETF"; last: 4.21; change: 0.03 }
                        }

                        delegate: Rectangle {
                            width: stockList.width
                            height: 52
                            color: ListView.isCurrentItem ? "#1f1f1f" : "#141414"

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    stockList.currentIndex = index
                                    // 右侧展示当前选中
                                    rightPanel.currentSymbol = symbol
                                    rightPanel.currentName = name
                                    rightPanel.currentPrice = last
                                    rightPanel.currentChange = change
                                }
                            }

                            Row {
                                anchors.fill: parent
                                anchors.margins: 8
                                spacing: 8

                                Column {
                                    spacing: 2
                                    Text {
                                        text: name
                                        color: "#ffffff"
                                        font.pixelSize: 14
                                        elide: Text.ElideRight
                                        width: 150
                                    }
                                    Text {
                                        text: symbol
                                        color: "#777777"
                                        font.pixelSize: 11
                                    }
                                }

                                Item { width: 10; height: 1 }

                                Column {
                                    anchors.right: parent.right
                                    anchors.rightMargin: 8
                                    anchors.verticalCenter: parent.verticalCenter
                                    spacing: 2

                                    Text {
                                        text: last.toFixed(2)
                                        color: change >= 0 ? "#f44336" : "#00e676"
                                        font.pixelSize: 14
                                    }
                                    Text {
                                        text: (change >= 0 ? "+" : "") + change.toFixed(2)
                                        color: change >= 0 ? "#f44336" : "#00e676"
                                        font.pixelSize: 11
                                        horizontalAlignment: Text.AlignRight
                                    }
                                }
                            }
                        }
                    }
                }

                // 右侧：K线 / 分时 / 详情区域（现在先做占位）
                Rectangle {
                    id: rightPanel
                    color: "#101010"
                    SplitView.fillWidth: true

                    // 对外暴露一些属性，方便中间列表赋值
                    property string currentSymbol: ""
                    property string currentName: ""
                    property real currentPrice: 0
                    property real currentChange: 0

                    Column {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 12

                        // 顶部大号报价区
                        Rectangle {
                            width: parent.width
                            height: 80
                            radius: 8
                            color: "#181818"

                            Row {
                                anchors.fill: parent
                                anchors.margins: 12
                                spacing: 16

                                Column {
                                    spacing: 4
                                    Text {
                                        text: rightPanel.currentName !== "" ? rightPanel.currentName : "请选择一只股票"
                                        color: "#ffffff"
                                        font.pixelSize: 20
                                    }
                                    Text {
                                        text: rightPanel.currentSymbol
                                        color: "#777777"
                                        font.pixelSize: 12
                                    }
                                }

                                Item { Layout.fillWidth: true }

                                Column {
                                    spacing: 4
                                    Text {
                                        text: rightPanel.currentPrice > 0 ? rightPanel.currentPrice.toFixed(2) : "--"
                                        color: rightPanel.currentChange >= 0 ? "#f44336" : "#00e676"
                                        font.pixelSize: 24
                                    }
                                    Text {
                                        text: rightPanel.currentChange !== 0 ?
                                              (rightPanel.currentChange >= 0 ? "+" : "") + rightPanel.currentChange.toFixed(2)
                                            : ""
                                        color: rightPanel.currentChange >= 0 ? "#f44336" : "#00e676"
                                        font.pixelSize: 12
                                        horizontalAlignment: Text.AlignRight
                                    }
                                }
                            }
                        }

                        // 中间：K线 / 分时占位
                        Rectangle {
                            id: chartArea
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: parent.width
                            height: parent.height - 80 - 120    // 顶部报价 + 底部预留
                            radius: 8
                            color: "#151515"

                            Text {
                                anchors.centerIn: parent
                                text: "这里将来放 K 线 / 分时 图表"
                                color: "#555555"
                            }
                        }
                    }
                }
            }

            // 底部状态栏
            Rectangle {
                id: statusBar
                Layout.fillWidth: true
                Layout.preferredHeight: 26
                color: "#181818"

                Row {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 16

                    Text {
                        text: "服务器: tcp://127.0.0.1:5555"
                        color: "#777777"
                        font.pixelSize: 11
                    }
                    Text {
                        text: "延迟: 5 ms"
                        color: "#777777"
                        font.pixelSize: 11
                    }
                    Text {
                        text: "最后更新: 14:35:12"
                        color: "#777777"
                        font.pixelSize: 11
                    }

                    Item { Layout.fillWidth: true }

                    Text {
                        text: "ZMQ 行情客户端 - 纯 QML 原型界面"
                        color: "#555555"
                        font.pixelSize: 10
                    }
                }
            }
        }
        
    }
}

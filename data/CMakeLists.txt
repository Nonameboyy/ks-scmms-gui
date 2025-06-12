set(SCMMS_GUI_ICON_INSTALL_PATH /usr/local/share/icons/hicolor)

install(FILES "images/16px.png"
	DESTINATION ${SCMMS_GUI_ICON_INSTALL_PATH}/16x16/apps RENAME ks-scmms-gui.png)

install(FILES "images/22px.png"
	DESTINATION ${SCMMS_GUI_ICON_INSTALL_PATH}/22x22/apps RENAME ks-scmms-gui.png)

install(FILES "images/24px.png"
	DESTINATION ${SCMMS_GUI_ICON_INSTALL_PATH}/24x24/apps RENAME ks-scmms-gui.png)

install(FILES "images/32px.png"
	DESTINATION ${SCMMS_GUI_ICON_INSTALL_PATH}/32x32/apps RENAME ks-scmms-gui.png)

install(FILES "images/48px.png"
	DESTINATION ${SCMMS_GUI_ICON_INSTALL_PATH}/48x48/apps RENAME ks-scmms-gui.png)

install(FILES "images/64px.png"
	DESTINATION ${SCMMS_GUI_ICON_INSTALL_PATH}/64x64/apps RENAME ks-scmms-gui.png)

install(FILES "images/96px.png"
	DESTINATION ${SCMMS_GUI_ICON_INSTALL_PATH}/96x96/apps RENAME ks-scmms-gui.png)

install(FILES "images/128px.png"
	DESTINATION ${SCMMS_GUI_ICON_INSTALL_PATH}/128x128/apps RENAME ks-scmms-gui.png)

install(FILES "images/logo.svg"
    DESTINATION ${SCMMS_GUI_ICON_INSTALL_PATH}/scalable/apps RENAME ks-scmms-gui.svg)

configure_file("ks-scmms-gui.desktop.in" "${CMAKE_CURRENT_BINARY_DIR}/ks-scmms-gui.desktop")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ks-scmms-gui.desktop" DESTINATION /usr/local/share/applications/)

%global pkg_name ks-scmms-gui
%global pkg_ver 1.3
%global pkg_rel 7

# no debug infos with:
%global debug_package %{nil}

# disable automatic dependency and provides generation with:
%define __find_provides %{nil}
%define __find_requires %{nil}
%define _use_internal_dependency_generator 0
%define with_prelink 0

Name:          %{pkg_name}
Version:       %{pkg_ver}
Release:       %{pkg_rel}%{dist}
Summary:       KylinSec SCMMS GUI Interface

License:        No License
URL:            http://gitlab.kylinos.com.cn/os/%{name}
Source0:        %{name}-%{version}.tar.gz
BuildRoot:      %_topdir/BUILDROOT

ExcludeArch:   i686
ExcludeArch:   armv7hl

BuildRequires: make
BuildRequires: cmake3 >= 3.6.1
BuildRequires: mesa-libGL-devel

%define make_run_pkg 0

%if "%{kylin_version}" == "3.2" 
%define make_run_pkg 1
BuildRequires: devtoolset-8-binutils
BuildRequires: devtoolset-8-runtime
BuildRequires: devtoolset-8-gcc
BuildRequires: devtoolset-8-gcc-c++
BuildRequires: devtoolset-8-libstdc++-devel
# 以下为libQt5WebEngineCore.so.5.7.1依赖
BuildRequires: fontconfig
BuildRequires: freetype
BuildRequires: libXi
BuildRequires: libXcursor
BuildRequires: libXrender
BuildRequires: libXcomposite
BuildRequires: alsa-lib
BuildRequires: libXtst
BuildRequires: libxslt
%else # not == "3.2"
BuildRequires: qt5-qtwebengine-devel
%if "%{kylin_version}" == "3.3"
BuildRequires: qt5-qtbase-devel
%ifarch aarch64
%define make_run_pkg 1
BuildRequires: python
BuildRequires: qt5-qtsvg
BuildRequires: fcitx-qt5
BuildRequires: libvpx >= 1.4
BuildRequires: qt5-qtx11extras
BuildRequires: qt5-qtwayland
BuildRequires: gsettings-qt
BuildRequires: qrencode-devel
BuildRequires: qt5-linguist
%endif # end with %ifarch aarch64
%else # not == "3.3"
BuildRequires: qt5-devel
%endif # end with %if "%{kylin_version}" == "3.3"
%endif # end with %if "%{kylin_version}" == "3.2"

%if "%{make_run_pkg}" != "1"
Requires: qt5-qtbase
Requires: qt5-qtwebengine
Requires: mesa-libGL
Requires: fcitx-qt5
Requires: socat
%endif

%description
%{summary}.

%prep
%setup -q -n %{name}-%{version}

%build
export SCMMS_FINAL_DIR=/usr/local/scmms/gui
export SCMMS_INSTALL_DIR=%{_builddir}/%{name}-%{version}${SCMMS_FINAL_DIR}
rm -rf ${SCMMS_INSTALL_DIR}
mkdir -p ${SCMMS_INSTALL_DIR}

%if "%{kylin_version}" == "3.2"
tar -xf ./buildrpm/gui.tar.gz --strip-components 1 -C ${SCMMS_INSTALL_DIR}
set +e
source scl_source enable devtoolset-8
export Qt5_DIR=${SCMMS_INSTALL_DIR}/5.7/gcc_64/lib/cmake/Qt5
export PATH=${SCMMS_INSTALL_DIR}/5.7/gcc_64/bin:$PATH
export LD_LIBRARY_PATH=${SCMMS_INSTALL_DIR}/lib64:${SCMMS_INSTALL_DIR}/5.7/gcc_64/lib:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=${SCMMS_INSTALL_DIR}/5.7/gcc_64/lib/pkgconfig:$PKG_CONFIG_PATH
#%else
%endif # end with %if "%{kylin_version}" == "3.2"

mkdir build
cd build
cmake3 -DCMAKE_INSTALL_PREFIX=${SCMMS_FINAL_DIR} ..
make

%install
export SCMMS_FINAL_DIR=/usr/local/scmms/gui
export SCMMS_INSTALL_DIR=%{buildroot}${SCMMS_FINAL_DIR}
rm -rf $RPM_BUILD_ROOT
cd build
%make_install
cd -

mkdir -p %{buildroot}/usr/bin
%if "%{make_run_pkg}" != "1"
cp ${SCMMS_INSTALL_DIR}/bin/ks-scmms-gui %{buildroot}/usr/bin
cp -aR $(which socat) ${SCMMS_INSTALL_DIR}/bin/socat
%else
cp ./buildrpm/ks-scmms-gui %{buildroot}/usr/bin
%if "%{kylin_version}" == "3.2"
SRC_QT=.$SCMMS_FINAL_DIR/5.7/gcc_64
DEST_QT=$SCMMS_INSTALL_DIR/5.7/gcc_64
mkdir -p $DEST_QT/lib
mv $SRC_QT/lib/*.so* $DEST_QT/lib
mv $SRC_QT/plugins $DEST_QT
mv $SRC_QT/libexec $DEST_QT
mv $SRC_QT/resources $DEST_QT
mv .$SCMMS_FINAL_DIR/lib64 $SCMMS_INSTALL_DIR
# already add to gui.tar.gz
# cp -aR ./buildrpm/lib*.so.* $SCMMS_INSTALL_DIR/lib64
cp -aR ./buildrpm/socat ${SCMMS_INSTALL_DIR}/bin/socat
%else
extract_file=./extract-libs/extract-libs.py
python $extract_file $deps_dir --need_qt_plugins --need_qt_resources ${SCMMS_INSTALL_DIR}
cp -aR ./buildrpm/socat.aarch64 ${SCMMS_INSTALL_DIR}/bin/socat
sed -i '/export SCMMS_INSTALL_DIR=/a\export QT_QPA_PLATFORM_PLUGIN_PATH=${SCMMS_INSTALL_DIR}/lib64/plugins' %{buildroot}/usr/bin/ks-scmms-gui
sed -i 's|:${SCMMS_INSTALL_DIR}/5.7/gcc_64/lib||g' %{buildroot}/usr/bin/ks-scmms-gui
%endif # end with %if "%{kylin_version}" == "3.2"
%endif # end with %if "%{make_run_pkg}" != "1"

%files
%doc
/usr/local/scmms/gui/*
/usr/bin/*
/usr/local/share/*

%post
# icons缓存刷新展示图标
gtk-update-icon-cache /usr/share/icons/hicolor
if [[ -d ~/Desktop/ ]];then
cp -f /usr/local/share/applications/ks-scmms-gui.desktop ~/Desktop/
chmod +x ~/Desktop/ks-scmms-gui.desktop
fi
if [[ -d ~/桌面/ ]];then
cp -f /usr/local/share/applications/ks-scmms-gui.desktop ~/桌面/
chmod +x ~/桌面/ks-scmms-gui.desktop
fi

%postun
xdg-desktop-icon uninstall --novendor %{_datadir}/applications/ks-scmms-gui.desktop
if [[ -e  ~/Desktop/ks-scmms-gui.desktop ]];then
rm -rf ~/Desktop/ks-scmms-gui.desktop
fi
if [[ -e  ~/桌面/ks-scmms-gui.desktop ]];then
rm -rf ~/桌面/ks-scmms-gui.desktop
fi

%clean
rm -rf $RPM_BUILD_ROOT

%changelog
* Fri Jun 30 2023 zhenggongping <zhenggongping@kylinsec.com.cn> - 2.3-1
- KYOS-F: first release


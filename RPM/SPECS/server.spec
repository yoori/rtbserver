%if %{?__target:1}%{!?__target:0}
%define _target_cpu             %{__target}
%endif
%if %{?__server_type:0}%{!?__server_type:1}
%define _target_cpu             noarch
%endif

%ifnarch noarch
%define __type                  %{?__server_type:%__server_type}%{!?__server_type:remote}
%endif

%define __product               server
%define __ob_ver_req            4.3.2
%define __inst_root             /opt/foros/%{__product}
%define __osbe_build_dir        .

%define __cms_plugin_name       %{__product}-plugin-%{version}
%define __cms_plugin_src_dir    %{__product}/CMS/Plugin
%define __cms_plugin_dst_dir    /opt/cms/var/spool

%define __apache_ver_req        1:2.2.21.4
%define __ace_ver_req           6.2.1.15-1.el6
#%define __glibc_ver_req         2.12-1.209.el6
%define __geoip_ver_req         1.4.8-ssv1.el6
%define __net_snmp_ver_req      5.5
%define __libevent_ver_req      1:1.4.13-11.ssv1.el6
%define __postgresql_ver_req    9.4.4
%define __protobuf_ver_req      2.5.0-2.ssv1.el6
%define __zeromq_ver_req        4.1.4-5.el6
%define __librdkafka_ver_req    0.11.3-ssv1.el6
%define __open_ssl_ver_req      1.0.1e-42.el6
%define __vanga_ver_req         1.0.0.5-ssv1.el6


Name:    foros-server%{?__type:-%__type}
Version: 2.7.0.0
Release: ssv1%{?dist}
Summary: Advertizing Server
License: Commercial
Group:   System Environment/Daemons

BuildRoot: %{_tmppath}/%{name}-buildroot
Source0: foros-server-%{version}.tar.gz

%ifnarch noarch
# updated on $Date: 2019-06-17 02:33:33 +0300 (Mon, 17 Jun 2019) $
BuildRequires: libstdc++ = 4.4.7-18.el6
Requires: libstdc++ = 4.4.7-18.el6
BuildRequires: glibc-devel
#BuildRequires: glibc-devel = %__glibc_ver_req
Requires: glibc
#Requires: glibc = %__glibc_ver_req

BuildRequires: ace-tao-devel >= %__ace_ver_req
Requires: ace-tao = %__ace_ver_req
BuildRequires: apr-devel >= 1:1.4.5-1.ssv1
BuildRequires: autoconf >= 2.63
BuildRequires: gcc-c++
BuildRequires: GeoIP-devel >= %__geoip_ver_req
Requires: GeoIP >= %__geoip_ver_req
Requires: foros-geoip >= 1.0.0.0
BuildRequires: httpd-devel >= %{__apache_ver_req}
Requires: httpd = %{__apache_ver_req}
Requires: nginx >= 1.8.1-1.el6
Requires: mod_ssl >= 2.2.21.4-1.ssv1
BuildRequires: libevent-devel = %__libevent_ver_req
Requires: libevent = %__libevent_ver_req
BuildRequires: OpenSBE = 1.0.54-ssv1
BuildRequires: OpenSBE-defs = 1.0.24.0
%if "%{?__type:%{__type}}%{!?__type:0}" == "central"
Conflicts: foros-server-remote-debuginfo
Conflicts: foros-server-central-debuginfo < %{version}
Conflicts: foros-server-central-debuginfo > %{version}
%else
Conflicts: foros-server-central-debuginfo
Conflicts: foros-server-remote-debuginfo < %{version}
Conflicts: foros-server-remote-debuginfo > %{version}
%endif
%if "%{?buildType}" == "nb" 
BuildRequires: valgrind-devel
%endif
BuildRequires: pcre-devel >= 7.8-6.el6
BuildRequires: prelink
BuildRequires: selinux-policy
BuildRequires: specspo
BuildRequires: xerces-c-devel >= 3.1.1
BuildRequires: xsd >= 4.0.0
BuildRequires: net-snmp-devel >= %{__net_snmp_ver_req}
Requires: net-snmp >= %{__net_snmp_ver_req}
Requires: net-snmp-subagent >= 2.1.3.1
BuildRequires: java-1.7.0-oracle-devel >= 1.7.0.45
BuildRequires: libxml2-devel = 2.7.8-7.el6
BuildRequires: libxslt-devel = 1.1.26-2.el6_3.1
BuildRequires: make >= 1:3.81-20.el6
BuildRequires: openssl-devel >= %{__open_ssl_ver_req}
BuildRequires: zlib-devel >= 1.2.3-29.el6
BuildRequires: bzip2-devel >= 1.0.5-7.el6_0
BuildRequires: flex >= 2.5.35-8.el6
BuildRequires: bison >= 2.4.1
BuildRequires: tokyocabinet-devel >= 1.4.43
BuildRequires: omniORB-devel >= 4.1.4
BuildRequires: omniORBpy-devel >= 3.4
BuildRequires: zeromq-devel = %__zeromq_ver_req
BuildRequires: librdkafka-devel = %__librdkafka_ver_req
BuildRequires: vanga = %__vanga_ver_req
Requires: postgresql94-libs >= %{__postgresql_ver_req}
Requires: openssl >= %{__open_ssl_ver_req}
BuildRequires: postgresql94-devel >= %{__postgresql_ver_req}
BuildRequires: protobuf-devel >= %{__protobuf_ver_req}
BuildRequires: protobuf-compiler >= %{__protobuf_ver_req}
Requires: protobuf >= %{__protobuf_ver_req}
Requires: foros-polyglot-dict >= 1.0.0.14-ssv1.el5
Requires: rsync >= 3.0.7-3.el6
Requires: stunnel >= 4.29
Requires: perl-Template-Toolkit
Requires: perl-Path-Iterator-Rule
Requires: perl-List-BinarySearch
Requires: perl-List-BinarySearch-XS
Requires: perl-Proc-Daemon
Requires: perl-Log-Dispatch
Requires: perl-Log-Dispatch-FileRotate
Requires: foros-dictionaries
Requires: zeromq = %__zeromq_ver_req
Requires: librdkafka1 = %__librdkafka_ver_req
Requires: vanga = %__vanga_ver_req
BuildRequires: boost-devel >= 1.57.0
BuildRequires: xgboost-devel

Provides: perl(Base) perl(Common) perl(TestHTML) perl(TestResultProc) perl(EmailNotifySend) perl(CampaignConfig) perl(CampaignTemplates) perl(DBEntityChecker) perl(PerformanceDB) perl(XMLEncoder)
%endif

%description
advertizing server main package
this package is for %{__type} colocations

%ifnarch noarch
%package  tests
Summary:  Test programs
Group:    System Environment/Daemons
Requires: postgresql94-libs >= %{__postgresql_ver_req}
Requires(pre): %{name} 
Provides: perl(ChannelConfig)

%if "%{?__type:%{__type}}%{!?__type:0}" == "central"
Requires: foros-server-central = %{version}
%else
Requires: foros-server-remote = %{version}
%endif

%description tests
advertizing server related test programs
%endif

%ifarch noarch
%package cms
Summary: AdServer CMS plugin
Group: System Environment/Daemons
Requires: cms
BuildRequires: zip

%description cms
advertizing server Configuration Management System plugin
%endif

%prep
%setup -q -n foros-server-%{version}

cpp_flags=''
if [ '%{?buildType}' == 'nb' ]; then cpp_flags='-DDEBUG '; fi

mkdir -p unixcommons/%{__osbe_build_dir}
cpp -DOS_%{_os_release} -DARCH_%{_target_cpu} -DARCH_FLAGS='%{__arch_flags}' ${cpp_flags} \
    unixcommons/default.config.t > unixcommons/%{__osbe_build_dir}/default.config

if [ '%__type' == 'central' ]; then cpp_flags+='-DUSE_OCCI'; fi
mkdir -p %{__product}/%{__osbe_build_dir}
cpp -DOS_%{_os_release} -DARCH_%{_target_cpu} -DARCH_FLAGS='%{__arch_flags}' ${cpp_flags} \
    -DUNIXCOMMONS_ROOT=%{_builddir}/foros-server-%{version}/unixcommons/%{__osbe_build_dir} \
    -DUNIXCOMMONS_INCLUDE=src \
    -DUNIXCOMMONS_CORBA_INCLUDE=src/CORBA \
    -DUNIXCOMMONS_DEF=%{_builddir}/foros-server-%{version}/unixcommons/libdefs \
    %{__product}/default.config.t > %{__product}/%{__osbe_build_dir}/default.config

%build
%ifnarch noarch
pushd unixcommons
osbe
product_root=`pwd`
cd %{__osbe_build_dir}
${product_root}/configure --enable-no-questions --enable-guess-location=no --prefix=%{__inst_root}
%{__make} %{_smp_mflags}
popd

pushd %{__product}
osbe
product_root=`pwd`
cd %{__osbe_build_dir}
${product_root}/configure --enable-no-questions --enable-guess-location=no --prefix=%{__inst_root}
%{__make} %{_smp_mflags}
popd
%endif

%ifarch noarch
pushd %{__cms_plugin_src_dir}
sed "/<application/,/>/s/\(\bversion=\)\"[^\"]\+\"/\1\"%{version}\"/" -i AdServerAppDescriptor.xml
zip -r %{__cms_plugin_name}.zip * -x .svn
popd
%endif

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{__inst_root}

%ifnarch noarch
make -C unixcommons/%{__osbe_build_dir} install destdir=%{buildroot}

rm -rf %{buildroot}%{__inst_root}/include
rm -rf `find %{buildroot}%{__inst_root}/lib -type f -name '*.a'`

make -C %{__product}/%{__osbe_build_dir} install destdir=%{buildroot}

# TODO: tests have files in xsd too
rm -f %{__product}.list
rm -f tests.list
test_dir="unixcommons/tests %{__product}/tests"
if [ "%{__osbe_build_dir}" != "." ]; then
  test_dir="$test_dir unixcommons/%{__osbe_build_dir}/tests %{__product}/%{__osbe_build_dir}/tests %{__product}/%{__osbe_build_dir}/utils"
fi
for f in `find %{buildroot}%{__inst_root} -type f`; do
  binary=`basename $f`
  inst_name=`echo "$f" | sed -e 's#%{buildroot}##g' -e 's#\.py$#\.py*#'`
  res=`find $test_dir -type f -name $binary`
    if [ -n "$res" ] || \
      [ "$binary" = "ChannelTest" ] || \
      [ "$binary" = "UserInfoAdmin" ] || \
      [ "$binary" = "ProfileDump" ] || \
      [ "$binary" = "UserInfoExchangerProxy" ] || \
      [ "$binary" = "ExtractMatchedChannels.pl" ] || \
      [ "$binary" = "ExtractCampaignClick.pl" ] || \
      [ "$binary" = "XslTransformAdmin" ] ; then
      echo "$inst_name" >> tests.list
    else
      echo "$inst_name" >> %{__product}.list
    fi
done
%endif

%ifarch noarch
mkdir -p %{buildroot}%{__cms_plugin_dst_dir}
install -m 0644 -T %{__cms_plugin_src_dir}/%{__cms_plugin_name}.zip %{buildroot}%{__cms_plugin_dst_dir}/%{__cms_plugin_name}.zip.rpm
%endif

%ifarch noarch
%post cms
ln -s %{__cms_plugin_name}.zip.rpm %{__cms_plugin_dst_dir}/%{__cms_plugin_name}.zip

%preun cms
rm -f %{__cms_plugin_dst_dir}/%{__cms_plugin_name}.zip ||:
%endif

%ifnarch noarch
%pre
# for clean upgrade from previous version
[ -h %__inst_root ] && rm  %__inst_root ||:

%files -f %{__product}.list
%defattr(-, root, root)
%dir %{__inst_root}/

%files tests -f tests.list
%defattr(-, root, root)
%endif

%ifarch noarch
%files cms
%defattr(-, root, root)
%{__cms_plugin_dst_dir}/%{__cms_plugin_name}.zip.rpm
%endif

%clean
rm -rf %{buildroot}

%changelog

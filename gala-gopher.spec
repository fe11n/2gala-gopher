Summary: Intelligent ops toolkit for openEuler
Name: gala-gopher
Version: 1.0.0
Release: 1
License: Mulan PSL v2
URL:       https://gitee.com/openeuler/gala-gopher
Source:    %{name}-%{version}.tar.gz
BuildRoot: %{_builddir}/%{name}-%{version}
BuildRequires: cmake
Buildrequires: gcc-c++
BuildRequires: yum
BuildRequires: elfutils-devel 
BuildRequires: clang >= 10.0.1
BuildRequires: llvm
BuildRequires: libconfig-devel librdkafka-devel libmicrohttpd-devel
BuildRequires: uthash-devel libbpf libbpf-devel log4cplus-devel
BuildRequires: java-1.8.0-openjdk-devel libcurl-devel
Requires: bash glibc elfutils zlib elfutils-devel bpftool iproute erlang-eflame libcurl

%description
gala-gopher is a low-overhead eBPF-based probes framework

%prep
%setup -q

%build
pushd build
sh build.sh --release
popd

%install
install -d %{buildroot}/opt/gala-gopher
install -d %{buildroot}%{_bindir}
mkdir -p  %{buildroot}/usr/lib/systemd/system
install -m 0600 service/gala-gopher.service %{buildroot}/usr/lib/systemd/system/gala-gopher.service
pushd build
sh install.sh %{buildroot}%{_bindir} %{buildroot}/opt/gala-gopher
popd
install -m 0600 /usr/lib64/erlang/lib/eflame-0/bin/flamegraph.pl %{buildroot}/%{_bindir}

%post
%systemd_post gala-gopher.service

%preun
%systemd_preun gala-gopher.service

%postun
if [ $1 -eq 0 ]; then
    rm -rf /sys/fs/bpf/gala-gopher >/dev/null
fi
%systemd_postun_with_restart gala-gopher.service

#%clean

%files
%defattr(-,root,root)
%dir /opt/gala-gopher
%dir /opt/gala-gopher/extend_probes
%dir /opt/gala-gopher/meta
%dir /opt/gala-gopher/lib
%{_bindir}/*
%config(noreplace) /opt/gala-gopher/*.conf
/opt/gala-gopher/extend_probes/*
%exclude /opt/gala-gopher/extend_probes/*.pyc
%exclude /opt/gala-gopher/extend_probes/*.pyo
/opt/gala-gopher/meta/*
/opt/gala-gopher/lib/*
/usr/lib/systemd/system/gala-gopher.service

%changelog
* Thu Sep 16 2022 zhaoyuxing<zhaoyuxing2@huawei.com> - 1.0.0-1
- Package init


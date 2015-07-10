<?xml version="1.0" encoding="utf-8"?>
<?CDF VERSION="6.0"?>
<DEFINITION>
  <SOFTPKG NAME="{02C4B96A-F9E6-4661-AD27-56571FE8C20A}" VERSION="1.0" TYPE="VISIBLE">
    <TITLE>MQTT Library</TITLE>
    <IMPLEMENTATION>
      <OS VALUE="Linux-ARMv7-A" />
      <CODEBASE FILENAME="LinuxARMV7A/liblvmqtt.so" TARGET="/usr/lib/liblvmqtt.so" PERMISSIONS="--x--x--x" />
    </IMPLEMENTATION>
    <IMPLEMENTATION>
      <OS VALUE="NI-Linux x64" />
      <CODEBASE FILENAME="LinuxX64/liblvmqtt.so" TARGET="/usr/lib/liblvmqtt.so" PERMISSIONS="--x--x--x" />
    </IMPLEMENTATION>
  </SOFTPKG>
</DEFINITION>
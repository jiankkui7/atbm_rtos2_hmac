@echo off
::set svn_path="E:\jiangyongjiu\rtos_hmac"
set svn_path=.\
set template_file=.\svn_version.h.template
set version_file=.\include\svn_version.h

::/closeonend��0���Զ��رնԻ���
::/closeonend��1���Զ��رգ����û�д���
::/closeonend��2���Զ��رգ����û�з�������ͳ�ͻ
::/closeonend��3���Զ��رգ����û�д��󣬳�ͻ�ͺϲ�
::/closeonend��4���Զ��رգ����û�д��󣬳�ͻ�ͺϲ�
TortoiseProc.exe /command:update /path:%svn_path% /closeonend:2

IF %ERRORLEVEL% NEQ 0 goto ERROR

echo �ļ����³ɹ�

subwcrev.exe  %svn_path% %template_file%    %version_file%

IF %ERRORLEVEL% NEQ 0 goto ERROR

echo SVN�汾���ļ�д��ɹ�

goto OK

:ERROR
echo ����ʧ�ܣ�����
pause
exit
:OK
pause
DEL fbs_generated\*
FOR %%i IN (fbs\*.*) DO flatc --cpp -o fbs_generated/ --filename-suffix "" %%i

DEL fbs_generated\* /F
FOR %%i IN (..\rust_pbrt_distributed\scene_file\fbs\*.*) DO flatc --gen-object-api --cpp -o fbs_generated/ --filename-suffix "" %%i

1.output.txt
    - Use IAR tool to compile the project. lpmcu_96cv300.bin is generated.
    - Use the tool hex2char.exe to convert the lpmcu_96cv300.bin to output.txt.
2.output_ca_release.txt
    - Modify HI_LOG_ENABLE in src/base.h to 0, and then compile the project. lpmcu_96cv300.bin is generated.
    - Use the tool hex2char.exe to convert the lpmcu_96cv300.bin to output.txt.
    - Rename the output.txt to output_ca_release.txt
3.How to use IAR compile project
    - Open lpmcu_96cv300.eww.
    - Right click on the project, select make to compile, or select rebuiid all to recompile.

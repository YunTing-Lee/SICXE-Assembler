# SICXE-Assembler

## 開發平台
Windows 10

## 開發環境
Code::Block20.03

## 說明
讀入使用者所事先定義好的Table1.table(instruction table), Table2.table(pseudo table), Table3.table(register table), Table4.table(delimeter table)與SIC/SICXE source code的檔案(SIC_input.txt)，將檔案裡的每道指令翻成機器碼，寫入到一個新的output檔中，可以使用的pseudo instruction有START、END、BYTE、WORD、RESB、RESW、EQU、BASE、LTORG。  
- START : 若第一個指令為START，則會指定starting address，否則預設為0000
- END : 做到END指令，代表整個程式已結束，若此時還有未定一的literal，會在END指令後定義。
- BYTE : 定義字元或16進位的常數
- WORD : 定義一個3byte的整數常數
- RESB : 保留所示數量的位元組
- RESW : 保留所示數量的word
- EQU : 定義、宣告一個symbol，此symbol的值即為該行敘述的位址。
- BASE : 做到BASE指令會將其後接的symbol位置或數字設定成BASE register的值。 
- LTORG : 做到LTORG時，會檢查LITTAB中是否還有未定義的literal，若有則會將這些literal定義在LTORG後面。


## 使用流程 :
1. 使用者一開始要先輸入SIC/SICXE source code的input檔名稱，不用加上附檔名，若使用者輸入的檔案不存在則會請使用者重新輸入檔名，直到這個檔案存在為止。
2. 程式將input檔翻譯完成後會將結果寫到一個新的檔案裡，output檔的名稱會自動命名為{原input檔名}+ "_output.txt"，例如原始input檔名稱為"SIC_input.txt"，則會將結果輸出到叫"SIC_input_output.txt"的檔案裡。

## 程式運作流程 : 
- pass1 : 將source program中有定義的label放入SYMTAB，判斷是否為duplicate symbol，並根據OPTAB查詢每行指令的長度或依照pseudo指示作對應動作，然後更新LOCCTR。
![image](https://github.com/YunTing-Lee/SICXE-Assembler/blob/main/Picture/SICXE%20pass1.png)

- pass2 : 若指令有用到symbol，則道SYMTAB找此symbol，若沒找到則發現undefined symbol，若有找到則取出此symbol對應的位址，然後根據指令format產生出對應格式的機器碼或依照pseudo instruction指示作對應動作及產生ascii碼。
![image](https://github.com/YunTing-Lee/SICXE-Assembler/blob/main/Picture/SICXE%20pass2.png)
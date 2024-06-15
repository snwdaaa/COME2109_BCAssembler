# COME2109_BCAssembler

## BCAssembler
- Basic Computer에서 작동하는 Assembler
- C로 구현
- Computer System Architecture(M. Morris Mano) 내용 참고

## 작동 방식
1. txt 파일에서 명령어 리스트 받아옴
2. first pass 수행 -> Address Symbol Table 작성
3. second pass 수행 -> 미리 작성한 Pseudo-Instruction Table, MRI Table, Non-MRI Table과 first pass에서 작성한 Address Symbol Table을 가지고 명령어를 기계어로 변환
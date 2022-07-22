#ifndef PEEPHOLE_HPP
#define PEEPHOLE_HPP

void Peephole_op(){
    std::ifstream file("RISCV-target/pre_asm.txt");
    freopen("RISCV-target/h.s", "w", stdout);
    if (file.is_open()) {
        std::string line;
        std::string temp;
        std::getline(file, line);
        temp = line;
        while (std::getline(file, line)) {
        // using printf() in all tests for consistency
            if (temp != "") printf("%s\n", temp.c_str());
            //int t = temp.compare(0,4,"\tsd ");
            if (temp.compare(0,4,"\tsd ") == 0 && line.compare(0,4,"\tld ") == 0 && temp.compare(4,10, line, 4,10) == 0){
                temp = "";
            }else{
                temp = line;
            }
            //More patterns can be added here
        }
    
        file.close();
    }
}




#endif
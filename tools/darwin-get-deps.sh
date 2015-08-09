otool -L obj/tools/qrview/qrview | grep "opt" | cut -d" " -f1 | xargs -I {} cp {} ./tmp/

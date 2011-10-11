require 'rake'
require 'rake/clean'

CC = 'g++'
MATH_LIB = "lib/libobido_collections.a"
SRCS = FileList["src/*.cpp"].each{|s| "src/" + s}
OBJS = SRCS.collect{|f| "obj/" + File.basename(f, ".cpp") + ".o"}
TESTS = FileList["tests/*.cpp"].each{|t| "tests/" + t}
FLAGS = '-Wall'


task :default => MATH_LIB do
    
end

TESTS.each{|t|
    file t => MATH_LIB do 
        exeFilePath = "bin/" + File.basename(t, ".cpp")
        libAbsolute = Dir.getwd + "/lib"
        sh "#{CC} #{FLAGS} -L\"#{libAbsolute}\"  #{t} -lobido_collections -o #{exeFilePath}"
    end
}

file MATH_LIB => OBJS do
       sh "ar cr #{MATH_LIB} #{OBJS.join(" ")}"
       sh "ranlib #{MATH_LIB}"    
end


OBJS.each{|obj|
    src = File.basename(obj, ".o") + ".cpp"
    file "#{obj}" => "src/#{src}" do
        sh %{#{CC} src/#{src} -c -o #{obj} #{FLAGS}}
    end
}



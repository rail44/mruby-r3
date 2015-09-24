MRuby::Gem::Specification.new('mruby-r3') do |spec|
  spec.license = 'MIT'
  spec.authors = 'AMEMIYA Satoshi'
  spec.summary = 'R3 router binding'

  FileUtils.mkdir_p build_dir
  r3_dir = build_dir + '/r3'
  lib_dir = r3_dir + '/.libs'

  Dir.chdir(build_dir) do
    `git clone --depth=1 https://github.com/c9s/r3.git`
  end unless Dir.exists?(r3_dir)

  Dir.chdir(r3_dir) do
    `./autogen.sh`
    `./configure`
    `make`
  end unless Dir.exists?(lib_dir)
  spec.cc.include_paths << "#{r3_dir}/include"
  spec.linker.flags_before_libraries << "#{lib_dir}/libr3.a"
  spec.linker.libraries << "pcre"
end

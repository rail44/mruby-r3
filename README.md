# mruby-r3   [![Build Status](https://travis-ci.org/rail44/mruby-r3.png?branch=master)](https://travis-ci.org/rail44/mruby-r3)
R3 class
## install by mrbgems
- add conf.gem line to `build_config.rb`

```ruby
MRuby::Build.new do |conf|

    # ... (snip) ...

    conf.gem :github => 'rail44/mruby-r3'
end
```
## example
```ruby
p R3.hi
#=> "hi!!"
t = R3.new "hello"
p t.hello
#=> "hello"
p t.bye
#=> "hello bye"
```

## License
under the MIT License:
- see LICENSE file

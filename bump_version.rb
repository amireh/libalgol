#!/usr/bin/env ruby

# bump_version.rb
# usage: ./bump_version.rb PART [STEP]
# where:
# => PART is one of [MAJOR|MINOR|PATCH|BUILD]
# => STEP is a positive number by which to bump (default: 1)
# if PART is BUILD, the STEP can be alphanumeric (ie, "b2")

ScriptName  = "bump_version.rb"

$stdout.sync = true

def log(msg, status = nil, newline = true)
  print "%s: %s#{status ? ' [ ' + status + ' ]' : ''}%s" % [ScriptName, msg, newline ? "\n" : '']
end


@@colorizing = true
begin
  gem 'colorize'
  require 'colorize'
rescue Exception => e
  log "couldn't find gem 'colorize', output will not be as pretty..."
  @@colorizing = false
end

def colorize_part(part)
  return part unless @@colorizing

  part.colorize(
    { "MAJOR" => :red, "MINOR" => :yellow, "PATCH" => :cyan, "BUILD" => :white }[part]).
    on_black.blink.underline
end

class Status
  OK      = @@colorizing ? "OK".green : "OK"
  FAILED  = @@colorizing ? "FAILED".red : "FAILED"
  NOTICE  = @@colorizing ? "NOTICE".light_blue : "NOTICE"
end

if ARGV.length < 1
  log "usage: ./bump_version.rb PART [STEP]", Status::FAILED
  Kernel.abort
end

cmake_file    = File.open("CMakeLists.txt")
doxygen_file  = File.open("Doxyfile")

@part = ARGV[0].upcase
@step = ARGV[1] || 1
@step = ARGV[1].to_i unless ARGV[1].nil? or @part == "BUILD"

def bumping_build?
  @part == "BUILD"
end

# locate the first _VERSION_ line in the CMakeLists.txt to figure out the prefix
prefix = nil
# the pattern to look for to mark the prefix
pattern = "_VERSION_#{@part}"

# the regex that will extract the current version value
matcher = bumping_build? ? /[a-z]+\d+/ : /[\d]+/

log "Doing a #{colorize_part(@part)} bump with #{@step} steps of a version..."
log "Processing CMakeLists.txt..."

cmake_file.read.each_line { |line|
  pos = line =~ /#{pattern}/; next if pos.nil?

  offset = "SET(".size
  prefix = line[offset..pos-1].strip

  project_name = prefix.downcase
  log 'Project name: %s/%s' % [prefix,project_name], Status::NOTICE
  log '--'

  new_value, value = @step, nil
  value = line[pos + pattern.size..line.size].gsub(matcher).first
  new_value = value.to_i + @step unless bumping_build?

  new_line = line.gsub(value, new_value.to_s)
  log "Found  directive: '%s'" % line.strip, Status::OK
  log "Bumped directive: '%s'" % new_line.strip, Status::OK

  log "[C]ontinue or [A]bort? ", nil, false

  answer = $stdin.gets
  answer = answer.chomp.upcase

  if answer != "C"
    log "Bye!"
    exit
  end

  old_content = File.read(cmake_file.path)
  cmake_file.close
  new_content = old_content.gsub(line, new_line)
  File.open("test/CMakeLists.txt", "w") { |f| f.write(new_content) }

  log "CMakeLists.txt has been modified.", Status::OK

  break
}


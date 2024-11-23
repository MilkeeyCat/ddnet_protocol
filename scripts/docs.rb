#!/usr/bin/env ruby

class FuncDoc
  attr_accessor :comment

  def initialize
    # documentation comment above the function
    @comment = ""

    # full signature of the function
    @signature = ""
  end

  def signature=(signature)
    @signature = signature.strip
  end

  def name
    @signature.match(/ (\w+)\(/)[1]
  end

  def valid?
    !@comment.empty? && !@signature.empty?
  end

  def to_markdown
    return "invalid" unless valid?

    markdown = ""
    markdown << "# #{name}\n\n"
    markdown << "## Syntax\n\n"
    markdown << "```C\n"
    markdown << @signature
    markdown << "\n"
    markdown << "```\n\n"
    # TODO: apply fancy markdown links to comment
    #       for example all types and functions in comment
    #       that are known ddnet_protocol types should be
    #       markdown links to their documentation entry
    markdown << "#{@comment}"
    markdown
  end
end

# @param filepath String
# @return FuncDoc[]
def parse_func_docs(filepath)
  func_docs = []
  func_doc = FuncDoc.new

  IO.foreach(filepath) do |line|
    next if line.strip.empty?
    next if line[0] == '#'

    if line.start_with? "// "
      func_doc.comment += line[3..]
      next
    end
    if line.strip == "//"
      func_doc.comment += "\n"
      next
    end

    # if its not a comment or a filtered line
    # we assume it has to be the function signature
    func_doc.signature = line
    func_docs << func_doc

    # create a empty doc obj for the next function
    func_doc = FuncDoc.new
  end
  func_docs
end

parse_func_docs("src/huffman.h").each do |func_doc|
  puts func_doc.to_markdown
  puts ""
end


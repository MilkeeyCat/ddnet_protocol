#!/usr/bin/env ruby

class BaseDoc
  attr_accessor :comment, :signature

  def initialize(comment)
    # documentation comment above the function
    @comment = comment
    @signature = ""
  end

  def comment_as_markdown
    # TODO: apply fancy markdown links to comment
    #       for example all types and functions in comment
    #       that are known ddnet_protocol types should be
    #       markdown links to their documentation entry
    @comment
  end

  def valid?
    !@comment.empty? && !@signature.empty?
  end
end

class StructDoc < BaseDoc
  def initialize(comment)
    super
  end

  def name
    @signature.split("\n").last.match(/ (\w+)/)[1]
  end

  def to_markdown
    return "invalid" unless valid?

    markdown = ""
    markdown << "# #{name}\n\n"
    markdown << "## Syntax\n\n"
    markdown << "```C\n"
    markdown << @signature
    markdown << "```\n\n"
    markdown << comment_as_markdown
    markdown
  end
end

class FuncDoc < BaseDoc
  attr_accessor :comment

  def initialize(comment, signature)
    super(comment)

    # full signature of the function
    @signature = signature.strip
  end

  def name
    @signature.match(/ \*?(\w+)\(/)[1]
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
    markdown << comment_as_markdown
    markdown
  end
end

def is_func?(line)
  !line.match(/(const )?(u?int(32|8)_t|Unpacker) \*?\w+\(/).nil?
end

def is_struct?(line)
  line.strip == "typedef struct {"
end

# @param filepath String
# @return (FuncDoc|StructDoc)[]
def parse_docs(filepath)
  docs = []
  comment = ""
  struct = nil

  IO.foreach(filepath) do |line|
    next if line.strip.empty?
    next if line[0] == '#'

    # seek till struct end if we got one
    if struct
      struct.signature += line
      if line.start_with? "} "
        docs << struct
        struct = nil
      end
      next
    end

    if line.start_with? "// "
      comment += line[3..]
      next
    end
    if line.strip == "//"
      comment += "\n"
      next
    end

    # if its not a comment or a filtered line
    # we assume we hit the C type that is being document
    # so either a struct or function signature
    if is_func? line
      docs << FuncDoc.new(comment, line)
    elsif is_struct? line
      # we have to seek to the end of the struct before
      # we can store the instance
      struct = StructDoc.new(comment)
      struct.signature += line
    else
      raise "Expected function or struct got unknown: #{line}"
    end

    # create a empty doc comment for next entry
    comment = ""
  end
  docs
end

def header_to_markdown(header_path, markdown_path)
  markdown = parse_docs(header_path).map do |doc|
    doc.to_markdown + "\n"
  end.inject(:+)
  File.write(markdown_path, markdown)
end

%w(packer huffman).each do |component|
  header_to_markdown("src/#{component}.h", "docs/#{component}.md")
end


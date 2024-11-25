#!/usr/bin/env ruby

class BaseDoc
  attr_accessor :comment, :signature

  DEFINE_VALUE_REGEX = /#define ([A-Z][A-Z_0-9]+) (0x)?[0-9]+$/

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

  def errors
    messages = []
    if @comment.empty?
      messages << "comment is empty"
    end
    if @signature.empty?
      messages << "signature is empty"
    end
    messages
  end

  def valid_or_throw(class_name)
    raise "#{class_name}: #{errors.join(", ")}" unless valid?
  end

  def self.is_func?(line)
    !line.match(/(const )?(u?int(32|8)_t|bool|Unpacker|Token|PacketHeader|PacketKind) \*?\w+\(/).nil?
  end

  def self.is_typedef?(line)
    return false if is_struct? line
    return false if is_enum? line

    !line.match(/^typedef /).nil?
  end

  def self.is_global_constant?(line)
    line.start_with? "extern const "
  end

  def self.is_define_value?(line)
     line.match?(DEFINE_VALUE_REGEX)
  end

  def self.is_struct?(line)
    line.strip == "typedef struct {"
  end

  def self.is_enum?(line)
    line.strip == "typedef enum {"
  end
end

class FuncDoc < BaseDoc
  def initialize(comment, signature)
    super(comment)

    # full signature of the function
    @signature = signature.strip
  end

  def name
    @signature.match(/ \*?(\w+)\(/)[1]
  end

  def to_markdown
    valid_or_throw(self.class.name)

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

class TypedefDoc < FuncDoc
  def name
    @signature.match(/ (\w+);$/)[1]
  end
end

class GlobalConstantDoc < TypedefDoc
  # OOPs where is the code
end

class DefineValueDoc < TypedefDoc
  def name
    @signature.match(DEFINE_VALUE_REGEX)[1]
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
    valid_or_throw(self.class.name)

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

class EnumDoc < StructDoc
  # OOPs where is the code
end

# @param filepath String
# @return (FuncDoc|StructDoc)[]
def parse_docs(filepath)
  docs = []
  comment = ""
  struct_or_enum = nil
  line_num = 0

  IO.foreach(filepath) do |line|
    line_num += 1
    next if line.strip.empty?
    next if line[0] == '#' && !BaseDoc.is_define_value?(line)

    # seek till struct end if we got one
    if struct_or_enum
      struct_or_enum.signature += line
      if line.start_with? "} "
        docs << struct_or_enum
        struct_or_enum = nil
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
    if BaseDoc.is_func? line
      docs << FuncDoc.new(comment, line)
    elsif BaseDoc.is_typedef? line
      docs << TypedefDoc.new(comment, line)
    elsif BaseDoc.is_global_constant? line
      docs << GlobalConstantDoc.new(comment, line)
    elsif BaseDoc.is_define_value? line
      docs << DefineValueDoc.new(comment, line)
    elsif BaseDoc.is_struct? line
      # we have to seek to the end of the struct before
      # we can store the instance
      struct_or_enum = StructDoc.new(comment)
      struct_or_enum.signature += line
    elsif BaseDoc.is_enum? line
      # we have to seek to the end of the enum before
      # we can store the instance
      struct_or_enum = EnumDoc.new(comment)
      struct_or_enum.signature += line
    else
      raise "#{filepath}:#{line_num} Expected function or struct got unknown: #{line}"
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

%w(packer huffman errors token packet).each do |component|
  header_to_markdown("src/#{component}.h", "docs/#{component}.md")
end


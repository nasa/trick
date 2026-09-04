class Trick < Formula
  desc "Simulation development framework"
  homepage "https://github.com/nasa/trick"
  url "https://github.com/nasa/trick/archive/refs/tags/25.1.1.tar.gz"
  sha256 "69a577668720ee9ab7fcf9312b486f53ba787f669432fa4c63bfc8e91b7b17da"
  license "NASA-1.3"
  head "https://github.com/nasa/trick.git", branch: "master"

  bottle do
    root_url "https://github.com/nasa/trick/releases/download/25.1.1"
    sha256 cellar: :any, arm64_tahoe:   "40769056581800d9a3bbb8642513971cf02a46bb7818012d98ef2c93c4d2898f"
    sha256 cellar: :any, arm64_sequoia: "ee0b3fb826156ab141e538b26b36f9a2e3146c4d8727ecf5bdc444d84883ad90"
  end

  LLVM_VERSION = "23".freeze

  depends_on "pkgconf" => :build
  depends_on arch: :arm64
  depends_on "bison"
  depends_on "flex"
  depends_on "gsl"
  depends_on "hdf5"
  depends_on "libaec"
  depends_on "libx11"
  depends_on "libxt"
  depends_on "llvm@23"
  depends_on :macos
  depends_on "maven"
  depends_on "openjdk"
  depends_on "openmotif"
  depends_on "perl"
  depends_on "python"
  depends_on "swig"
  depends_on "udunits"
  depends_on "zlib"

  uses_from_macos "libxml2"

  def install
    llvm = Formula["llvm@#{LLVM_VERSION}"]
    args = [
      "--with-gsl=#{formula_opt_prefix("gsl")}",
      "--with-hdf5=#{formula_opt_prefix("hdf5")}",
      "--with-udunits=#{formula_opt_prefix("udunits")}",
      "--with-llvm=#{llvm.opt_prefix}",
    ]

    system "./configure", *args, *std_configure_args
    system "make", "-j#{ENV.make_jobs}"

    bin.install Dir["bin/*"] if File.directory?("bin")
    lib.install Dir["lib/*"] if File.directory?("lib")
    include.install Dir["include/*"] if File.directory?("include")
    share.install Dir["share/*"] if File.directory?("share")
    libexec.install Dir["libexec/*"] if File.directory?("libexec")

    # Install trick_source/er7_utils
    Dir["trick_source/er7_utils/*/include/*.hh"].each do |f|
      dest = prefix/Pathname(f).dirname
      dest.mkpath
      cp f, dest
    end

    # Clean up build artifacts
    rm_r(Dir[prefix/"trick_source/**/object_*"])
    rm_r(Dir[prefix/"trick_source/**/io_src"])

    inreplace pkgshare/"makefiles/config_user.mk" do |s|
      # Fix hardcoded shim tool paths in installed config file
      s.gsub! %r{#{HOMEBREW_SHIMS_PATH}/[^/]+/}o, ""
      # Replace javac with homebrew openjdk javac path
      s.gsub! "javac", "#{formula_opt_bin("openjdk")}/javac"
    end

    # Fix HDF5 library paths to include libaec for libsz
    inreplace pkgshare/"makefiles/Makefile.common",
      "HDF5_LIB := -L$(HDF5)/lib -lhdf5_hl -lhdf5 -lsz",
      "HDF5_LIB := -L$(HDF5)/lib -L#{formula_opt_lib("libaec")} -lhdf5_hl -lhdf5 -lsz"
  end

  test do
    output = shell_output("#{bin}/trick-CP --help", 1)
    assert_match "Trick Configuration Processor", output
  end
end

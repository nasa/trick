class Trick < Formula
  desc "Simulation development framework"
  homepage "https://github.com/nasa/trick"
  url "https://github.com/nasa/trick/archive/refs/tags/25.1.0.tar.gz"
  sha256 "e4dcbc5e618e2805d9560ef5b7cacb1cd98a360fae5b69272ce72a7ba076dbb2"
  license "NASA-1.3"
  revision 1
  head "https://github.com/nasa/trick.git", branch: "master"

  bottle do
    root_url "https://github.com/nasa/trick/releases/download/25.1.0"
    sha256 cellar: :any, arm64_tahoe:   "094dc0c7cd02288b3626910f79fbd402d4a18851315b8ae36ea4a6bfc1b97b5e"
    sha256 cellar: :any, arm64_sequoia: "a1e4e084ffa19317040ba790c11eddb9809336c7e02a0bf600462fabf9aec109"
  end

  LLVM_VERSION = "22".freeze

  depends_on "pkgconf" => :build
  depends_on arch: :arm64
  depends_on "bison"
  depends_on "flex"
  depends_on "gsl"
  depends_on "hdf5"
  depends_on "libaec"
  depends_on "libx11"
  depends_on "libxt"
  depends_on "llvm@22"
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
      "--with-gsl=#{Formula["gsl"].opt_prefix}",
      "--with-hdf5=#{Formula["hdf5"].opt_prefix}",
      "--with-udunits=#{Formula["udunits"].opt_prefix}",
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
      # Fix hardcoded shim compiler paths in installed config file
      s.gsub! "super/clang++", "clang++"
      s.gsub! "super/clang", "clang"
      s.gsub! "super/ld", "ld"
      s.gsub! %r{#{HOMEBREW_SHIMS_PATH}/[^/]+/}o, ""
      # Replace javac with homebrew openjdk javac path
      s.gsub! "javac", "#{Formula["openjdk"].opt_bin}/javac"
    end

    # Fix HDF5 library paths to include libaec for libsz
    inreplace pkgshare/"makefiles/Makefile.common",
      "HDF5_LIB := -L$(HDF5)/lib -lhdf5_hl -lhdf5 -lsz",
      "HDF5_LIB := -L$(HDF5)/lib -L#{Formula["libaec"].opt_lib} -lhdf5_hl -lhdf5 -lsz"
  end

  test do
    output = shell_output("#{bin}/trick-CP --help", 1)
    assert_match "Trick Configuration Processor", output
  end
end

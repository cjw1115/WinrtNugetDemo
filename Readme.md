Nuget 是 dotnet 开发中必不可少的包管理工具，但不仅仅局限于 dotnet 项目，在 VS 中使用 C++ 开发的时候，也可以使用 Nuget 来引用第三方组件。同样也可以用 Nuget 把 native 的一些组件打包，提供给自己或者别人使用。

这片博文记录一下如何把 WinRT 组件打包成 nuget 包，供 UWP 项目使用。

之前提供给合作伙伴我们的 WinRT 组件时，是直接把 .winmd文件 和 .dll文件发给他们，其中 .winmd 提供声明，.dll 提供具体的实现。然后使用者就可以手动引用和复制这两个文件到项目里面使用。

但通过 Nuget 包的形式，一方面便于管理和发布，另一方面可以很好的做到版本控制。

实战如下：

## 准备原材料

### Demo.winmd

主要包含命名空间，类之类的声明，因为本质上使用的是 COM 技术，所以即使只添加对 .winmd 的引用，UWP 项目依旧可以成功编译。

### Demo.dll

包含对应 .winmd 的具体 COM 实现。

### Nuget.exe

用来打包 nuget 包，可以从 nuget 官网下载（https://www.nuget.org/downloads ）

## 创建 Nuget 包声明文件（.nuspec）

可以使用如下命令来生成一个名为 demo.nuspec 的模板文件

    nuget spec demo

在此文件中，需要加入对 Demo.winmd 和 Demo.dll 文件的引用，修改完成后如下

    <?xml version="1.0"?>
    <package >
        <metadata>
            <id>com.cq.nugetdemo</id>
            <version>1.0.0.0</version>
            <title>Nuget Demo</title>
            <authors>cq</authors>
            <owners>cq</owners>
            <requireLicenseAcceptance>false</requireLicenseAcceptance>
            <description>Just a test</description>
            <releaseNotes>Summary of changes made in this release of the package.</releaseNotes>
            <copyright>Copyright 2019</copyright>
            <tags>demo</tags>
        </metadata>
        <files>
            <!-- WinMd and IntelliSense files -->
            <file src=".\Nuget.winmd" target="lib\uap10.0"/>
            
            <!-- DLLs and resources -->
            <file src=".\Nuget.dll" target="runtimes\win10-x64\native"/>
            
            <!-- .targets -->
            <file src="com.cq.nugetdemo.targets" target="build\native"/>
        </files>
    </package>

metadata 节点提供一些必要的 nuget 包信息声明，最重要的是 id，这个要保证唯一性。

引用文件的位置是以 demo.nuspec 文件的位置为基准，可以使用相对路径。

需要将 nuget.winmd 文件放入 lib 文件夹下的 uap10.0 目录，其中 lib 文件夹会在最终生成的 nuget 包中创建，uap10.0 代表此文件只会被 UWP 应用引用。

需要将 demo.dll 文件放入 runtimes 文件夹下，同样这个文件夹我们不需要手动创建，它会在最终生成的 nuget 包中出现。win10-x64 代表该 dll 为 x64 cpu 架构，win10-x86 就代表32位，native 则表示此 dll的类型。

com.cq.nugetdemo.targets 文件和 demo.nuspec 文件在同一目录，它主要是为了让 C++ 的 UWP 项目也能正确添加对 nuget 包中 dll 和 winmd 文件的引用，在 C# 项目里面IDE会帮我们添加。 该文件如下：

    <?xml version="1.0" encoding="utf-8"?>
    <Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
        <PropertyGroup>
            <NugetDemo-Platform Condition="'$(Platform)' == 'Win32'">x86</NugetDemo-Platform>
            <NugetDemo-Platform Condition="'$(Platform)' != 'Win32'">$(Platform)</NugetDemo-Platform>
        </PropertyGroup>
        <ItemGroup Condition="'$(TargetPlatformIdentifier)' == 'UAP'">
            <Reference Include="$(MSBuildThisFileDirectory)..\..\lib\uap10.0\Nuget.winmd">
                <Implementation>Demo.dll</Implementation>
            </Reference>
            <ReferenceCopyLocalPaths Include="$(MSBuildThisFileDirectory)..\..\runtimes\win10-$(NugetDemo-Platform)\native\Demo.dll" />
        </ItemGroup>
    </Project>

虽然 C# 的 UWP 项目不需要 .targets 文件，但是为了让 IDE 能正确添加对 DLL 文件的引用，我们需要保持 dll 和 winmd 文件同名，否则在运行时会抛出异常。在编译时，会有如下警告：

    warning APPX1707: No implementation file was provided for the .winmd file 'C:\Users\...\.nuget\packages\com.cq.nugetdemo\1.0.0.3\lib\uap10.0\demo.winmd'. To generate registration information in the app manifest, specify the 'Implementation' metadata on the .winmd reference item in the project file.

## 创建 nuget 包（.nupkg)

    nuget.exe pack NugetDemo.nuspec -Version 1.0.0.3

在创建的时候，可以指定版本号，这方便我们用自动化脚本来生成 nuget 包。执行此命令后，在demo.nuspec 同级目录下会生成对应的 .nupkg。

## 使用 nuget 包

正常使用 nuget 包时，可以通过 VS 里的 nuget 包管理器去安装。但是如果我们的 nuget 包只想本地使用，那我们可以给 nuget 包管理器添加一个本地的包源，

在 .sln 同级目录，创建一个 Nuget.config 文件，然后可以在里面配置本地的 nuget source

    <?xml version="1.0" encoding="utf-8"?>
    <configuration>
        <packageSources>
            <add key="MyLocalSource" value=".\NugetPackage" />
        </packageSources>
    </configuration>

其中 MyLocalSource 是包源的名称，.\NugetPackage 是包源的路径，也是相对路径

现在就可以在包管理器中选择对应的本地源，然后安装生成的 .nupkg

## 参考

我主要参考了微软的官方文档：

https://docs.microsoft.com/en-us/nuget/guides/create-uwp-packages

微软文档会更加全面一些，而这篇博客主要是我的具体实践，对比该文档去掉了一些 暂时没发现有什么用 的步骤，添加了一些值得注意的点。

微软文档里提到的如下两点，在这个 demo 里没有用到：

### Package Resource Index (PRI) file

[微软文档](https://docs.microsoft.com/en-us/windows/uwp/app-resources/resource-management-system#package-resource-index-pri-file)

Every app package should contain a binary index of the resources in the app. This index is created at build time and it is contained in one or more Package Resource Index (PRI) files.

* A PRI file contains actual string resources, and an indexed set of file paths that refer to various files in the package.
* A package typically contains a single PRI file per language, named resources.pri.
* The resources.pri file at the root of each package is automatically loaded when the ResourceManager is instantiated.
* PRI files can be created and dumped with the tool MakePRI.exe.
* For typical app development you won't need MakePRI.exe because it's already integrated into the Visual Studio compile workflow. And Visual Studio supports editing PRI files in a dedicated UI. However, your localizers and the tools they use might rely upon MakePRI.exe.
* Each PRI file contains a named collection of resources, referred to as a resource map. When a PRI file from a package is loaded, the resource map name is verified to match the package identity name.
* PRI files contain only data, so they don't use the portable executable (PE) format. They are specifically designed to be data-only as the resource format for Windows. They replace resources contained within DLLs in the Win32 app model.
* The size limit on a PRI file is 64 kilobytes.

简而言之，PRI 文件相当于传统 Win32 开发中的 资源 DLL 文件。 

### XML documentation （Demo.xml）

在 IDE 的 C++ output file 属性页面，可以打开如下选项

    Generate XML Documentation files (/doc)

打开之后，就会生成和我们的 native dll 同名的 .xml 文件，大致如下：

    <?xml version="1.0"?>
    <doc>
        <assembly>
            <name>ClassLibrary</name>
        </assembly>
        <members>
            <member name="T:ClassLibrary.Demo">
                <summary>
                A demo how to use documentation.
                </summary>
            </member>
            <member name="M:ClassLibrary.Demo.GetDate">
                <summary>
                Get date of now.
                </summary>
                <returns>a date time struct</returns>
            </member>
        </members>
    </doc>

当把这个 .xml 和 .dll 同事分发给使用者时，使用者在 VS IDE 里面就能获得对代码注释的 智能感知 能力。当然这要求我们在提供编写 .dll 的源代码的时候，提供规定格式的良好的注释。

注释格式可以参考如下：

C++ https://docs.microsoft.com/en-us/cpp/build/reference/xml-documentation-visual-cpp?view=vs-2019

Dotnet(C#) https://docs.microsoft.com/en-us/dotnet/csharp/codedoc


## One more thing

事实上在 demo.dll 是一个 COM 服务器，使用前需要先注册，在 UWP 项目里如何注册这个 COM 服务器呢？

当我们添加对 .winmd 和 .dll 的引用后，在 UWP 的 AppMenifest 文件里面会自动添加如下节点：

    <Package>
        <Extensions>
            <Extension Category="windows.activatableClass.inProcessServer">
                <InProcessServer>
                    <Path>Nuget.dll</Path>
                    <ActivatableClass ActivatableClassId="Nuget.Class" ThreadingModel="both" />
                </InProcessServer>
            </Extension>
        </Extensions>
    </Package>


欢迎关注我的公众号

![CQInfo](https://github.com/cjw1115/Blogs/blob/master/CQInfo.jpg)

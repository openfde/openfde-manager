package main

import (
	"fmt"
	"io/ioutil"
	"os"
	"os/exec"
	"path/filepath"

	"github.com/godbus/dbus/v5"
	"openfde.service/security"
)

// OpenFDEService 定义一个结构体来实现 D-Bus 接口方法
type OpenFDEService struct{}

func (s *OpenFDEService) Utils(command string) (string, *dbus.Error) {
	cmd := exec.Command("bash", "fde_utils", "screenshot")
	output, err := cmd.Output()
	if err != nil {
		fmt.Println("capture_screenshot failed", nil, err)
		return "failed", nil
	}
	fmt.Println("capture_screenshot successful")

	err = ioutil.WriteFile("/tmp/openfde_screen.jpg", output, 0644)
	if err != nil {
		fmt.Println("write_screenshot_file failed", nil, err)
		return "failed", nil
	}
	return "", nil
}

func (s *OpenFDEService) Clear(homeDir string) (string, *dbus.Error) {
	targetDir := filepath.Join(homeDir, ".local", "share", "openfde")
	err := os.RemoveAll(targetDir)
	if err != nil {
		return fmt.Sprintf("Failed to remove directory %s: %v", targetDir, err), nil
	}

	return "cleared", nil
}

func (s *OpenFDEService) Tools(command string) (string, *dbus.Error) {
	cmd := exec.Command("bash", "get_fde.sh", "--"+command)
	// 执行脚本并捕获输出
	out, err := cmd.Output()
	if err != nil {
		fmt.Printf("error in tools "+command+" %v \n", err)
		return fmt.Sprintf("FailedExitCode: %d", cmd.ProcessState.ExitCode()), nil
	}
	fmt.Printf("in tools after exec  " + command + "\n")
	if command == "status" || command == "version_fde" || command == "version_ctrl" {
		return string(out), nil
	}
	return "complete", nil
}

func (s *OpenFDEService) Security(command string) (string, *dbus.Error) {
	if command == "disable" {
		fmt.Println("disable security")
		err := security.Disable()
		if err != nil {
			fmt.Println("disable security failed", err)
			return "failed", nil
		}
		return "complete", nil
	}
	if command == "query" {
		fmt.Println("query security")
		if security.Query() {
			fmt.Println("query security enabled")
			return "enabled", nil
		}
		return "disabled", nil
	}
	return "invalid command", nil
}

func (s *OpenFDEService) Construct() (string, *dbus.Error) {
	_, err := os.Stat("/tmp/get-openfde.sh")
	if err != nil {
		if os.IsNotExist(err) {
			fmt.Println("not exist")
			dbuserr := dbus.Error{
				Name: "org.freedesktop.DBus.Error.NotExist",
			}
			return fmt.Sprintf("script is not exist %v", err), &dbuserr
		}
		return fmt.Sprintf("script else is not exist %v", err), nil
	}
	// Execute sed command to modify the script
	sedCmd := exec.Command("sed", "-i", "/key/s/phyvirt.//g", "/tmp/get-openfde.sh")
	if err := sedCmd.Run(); err != nil {
		return fmt.Sprintf("Failed to modify script with sed: %v", err), nil
	}
	cmd := exec.Command("bash", "/tmp/get-openfde.sh")
	// 执行脚本并捕获输出
	_, err = cmd.Output()
	if err != nil {
		return fmt.Sprintf("Failed to execute /tmp/get-openfde.sh: %v", err), nil
	}

	// 打印脚本的输出
	return fmt.Sprintf("installed"), nil
}

func main() {
	// 连接到系统总线
	conn, err := dbus.ConnectSystemBus()
	if err != nil {
		fmt.Println("Failed to connect to system bus:", err)
		return
	}
	defer conn.Close()

	// 请求一个 D-Bus 名称
	name := "com.openfde.Manager"
	reply, err := conn.RequestName(name, dbus.NameFlagDoNotQueue)
	if err != nil {
		fmt.Println("Failed to request name: com.openfde.Manager", err)
		return
	}
	if reply != dbus.RequestNameReplyPrimaryOwner {
		fmt.Println("com.openfde.Manager Name already taken:", name)
		return
	}

	// 注册对象和接口
	objectPath := dbus.ObjectPath("/com/openfde/Manager")
	iface := "openfde.service"
	if err := conn.Export(&OpenFDEService{}, objectPath, iface); err != nil {
		fmt.Println("Failed to export openfde object:", err)
		return
	}
	fmt.Println("openfde D-Bus service registered successfully. Waiting for requests...")
	// 进入主循环，处理 D-Bus 消息
	select {}
}

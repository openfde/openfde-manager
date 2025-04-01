package security

import (
	"fmt"
	"os"
	"strings"
)

type Security interface {
	Disable() error
	Query() bool
}

func newSecurity() (Security, error) {
	data, err := os.ReadFile("/etc/os-release")
	if err != nil {
		return nil, err
	}
	var ID string
	lines := strings.Split(string(data), "\n")
	for _, line := range lines {
		if strings.HasPrefix(line, "ID=") {
			ID = strings.TrimPrefix(line, "ID=")
		}
	}
	if ID == "kylin" {
		return new(KylinSecurity), nil
	}
	if ID == "uos" {
		return new(UosSecurity), nil
	}
	return nil, fmt.Errorf("ID field not found in /etc/os-release")
}

func Query() bool {
	sec, err := newSecurity()
	if err != nil {
		return false
	}
	return sec.Query()
}

func Disable() error {
	sec, err := newSecurity()
	if err != nil {
		return nil
	}
	return sec.Disable()
}

/*
 Navicat Premium Data Transfer

 Source Server         : linux
 Source Server Type    : MySQL
 Source Server Version : 50733
 Source Host           : 192.168.142.128:3306
 Source Schema         : YOSHIKI_DB

 Target Server Type    : MySQL
 Target Server Version : 50733
 File Encoding         : 65001

 Date: 25/02/2023 04:40:37
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for Y_user
-- ----------------------------
DROP TABLE IF EXISTS `Y_user`;
CREATE TABLE `Y_user`  (
  `y_username` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `y_sex` varchar(8) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL,
  `y_id` int(11) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`y_id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 5 CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of Y_user
-- ----------------------------
INSERT INTO `Y_user` VALUES ('狗蛋', 'woman', 3);
INSERT INTO `Y_user` VALUES ('狗蛋', 'woman', 4);

SET FOREIGN_KEY_CHECKS = 1;

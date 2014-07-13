DELIMITER $$
CREATE DEFINER=`root`@`localhost` FUNCTION `klasse_berechnen`(`vajahr` SMALLINT) RETURNS int(11)
BEGIN IF MONTH(CURDATE()) >= 9 THEN RETURN(13+YEAR(CURDATE())-`vajahr`); ELSE RETURN(12+YEAR(CURDATE())-`vajahr`); END IF; END$$
DELIMITER ;

CREATE TABLE `buch` (
  `isbn` bigint(20) NOT NULL,
  `name` varchar(50) NOT NULL,
  `jgst` varchar(10) DEFAULT NULL,
  PRIMARY KEY (`isbn`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `lehrer` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(50) NOT NULL,
  `kuerzel` varchar(4) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `schueler` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(50) NOT NULL,
  `vajahr` int(11) DEFAULT NULL,
  `kbuchst` varchar(3) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `sausleihe` (
  `sid` int(11) NOT NULL,
  `bid` bigint(20) NOT NULL,
  `anz` smallint(6) NOT NULL DEFAULT '1',
  `adatum` date DEFAULT NULL,
  PRIMARY KEY (`sid`,`bid`),
  KEY `fk_bid` (`bid`),
  CONSTRAINT `sausleihe_ibfk_1` FOREIGN KEY (`sid`) REFERENCES `schueler` (`id`),
  CONSTRAINT `sausleihe_ibfk_2` FOREIGN KEY (`bid`) REFERENCES `buch` (`isbn`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `lausleihe` (
  `lid` int(11) NOT NULL,
  `bid` bigint(20) NOT NULL,
  `anz` smallint(6) NOT NULL DEFAULT '1',
  `adatum` date DEFAULT NULL,
  PRIMARY KEY (`lid`,`bid`),
  KEY `fk_bid` (`bid`),
  CONSTRAINT `lausleihe_ibfk_1` FOREIGN KEY (`lid`) REFERENCES `lehrer` (`id`),
  CONSTRAINT `lausleihe_ibfk_2` FOREIGN KEY (`bid`) REFERENCES `buch` (`isbn`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE `btausch` (
  `sid` int(11) NOT NULL,
  `bid` bigint(20) NOT NULL,
  `datum` date DEFAULT NULL,
  PRIMARY KEY (`sid`,`bid`),
  KEY `fk_bid` (`bid`),
  CONSTRAINT `btausch_ibfk_1` FOREIGN KEY (`sid`) REFERENCES `schueler` (`id`),
  CONSTRAINT `btausch_ibfk_2` FOREIGN KEY (`bid`) REFERENCES `buch` (`isbn`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE ALGORITHM=UNDEFINED DEFINER=CURRENT_USER() SQL SECURITY DEFINER VIEW `biblio`.`Btausch` AS select `biblio`.`btausch`.`sid` AS `a`,`biblio`.`btausch`.`sid` AS `b`,`biblio`.`btausch`.`bid` AS `bid`,`biblio`.`btausch`.`datum` AS `datum` from `biblio`.`btausch`;
CREATE ALGORITHM=UNDEFINED DEFINER=CURRENT_USER() SQL SECURITY DEFINER VIEW `biblio`.`Buch` AS select `biblio`.`buch`.`isbn` AS `isbn`,ifnull(concat(`biblio`.`buch`.`name`,' ',`biblio`.`buch`.`jgst`),`biblio`.`buch`.`name`) AS `titel` from `biblio`.`buch`;
CREATE ALGORITHM=UNDEFINED DEFINER=CURRENT_USER() SQL SECURITY DEFINER VIEW `biblio`.`SSchueler` AS select `biblio`.`schueler`.`id` AS `id`,concat(cast(`klasse_berechnen`(`biblio`.`schueler`.`vajahr`) as char charset utf8),ifnull(`biblio`.`schueler`.`kbuchst`,'')) AS `Klasse`,`biblio`.`schueler`.`name` AS `Name` from `biblio`.`schueler`;
CREATE ALGORITHM=UNDEFINED DEFINER=CURRENT_USER() SQL SECURITY DEFINER VIEW `biblio`.`Sausleihe` AS select `biblio`.`sausleihe`.`sid` AS `a`,`biblio`.`sausleihe`.`sid` AS `b`,`biblio`.`sausleihe`.`bid` AS `bid`,`biblio`.`sausleihe`.`anz` AS `anz`,`biblio`.`sausleihe`.`adatum` AS `adatum` from `biblio`.`sausleihe`;


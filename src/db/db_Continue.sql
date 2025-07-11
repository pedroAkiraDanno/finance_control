




CREATE TABLE coutrys (
    id SERIAL PRIMARY KEY,                   -- Auto-incrementing ID
    code VARCHAR(10) UNIQUE NOT NULL,        -- Unique country code
    name VARCHAR(100) NOT NULL,              -- Country name
    include_date DATE DEFAULT CURRENT_DATE,  -- Automatically set to today
    alter_date DATE                          -- Can be updated manually or via trigger
);


-- Step 1: Create the trigger function
CREATE OR REPLACE FUNCTION update_alter_date()
RETURNS TRIGGER AS $$
BEGIN
    NEW.alter_date := CURRENT_DATE;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Step 2: Attach the trigger to the table
CREATE TRIGGER set_alter_date
BEFORE UPDATE ON coutrys
FOR EACH ROW
EXECUTE FUNCTION update_alter_date();







INSERT INTO coutrys (code, name) VALUES
('AFE', 'AFEGANISTAO'),
('AFR', 'AFRICA DO SUL'),
('AGL', 'ANGUILLA'),
('ALB', 'ALBANIA, REPUBLICA DA'),
('ALE', 'ALEMANHA'),
('AND', 'ANDORRA'),
('ANG', 'ANGOLA'),
('ANT', 'ANTILHAS HOLANDESAS'),
('ARA', 'ARABIA SAUDITA'),
('ARG', 'ARGENTINA'),
('ARL', 'ARGELIA'),
('ARM', 'ARMENIA, REPUBLICA DA'),
('ARU', 'ARUBA'),
('AST', 'AUSTRIA'),
('ATG', 'ANTIGUA E BARBUDA'),
('AUS', 'AUSTRALIA'),
('AZE', 'AZERBAIJAO, REPUBLICA DO'),
('BAE', 'BAHREIN, ILHAS'),
('BAH', 'BAHAMAS, ILHAS'),
('BAN', 'BANGLADESH'),
('BAR', 'BARBADOS'),
('BEL', 'BELGICA'),
('BEN', 'BENIN'),
('BER', 'BERMUDAS'),
('BEU', 'BELARUS, REPUBLICA DA'),
('BEZ', 'BELIZE'),
('BFA', 'BURKINA FASO'),
('BOL', 'BOLIVIA'),
('BOS', 'BOSNIA-HERZEGOVINA (REPUBLICA DA)'),
('BOT', 'BOTSUANA'),
('BRA', 'BRASIL'),
('BRU', 'BRUNEI'),
('BUL', 'BULGARIA, REPUBLICA DA'),
('BUR', 'BURUNDI'),
('BUT', 'BUTAO'),
('CAB', 'CABO VERDE, REPUBLICA DE'),
('CAF', 'REPUBLICA CENTRO-AFRICANA'),
('CAL', 'NOVA CALEDONIA'),
('CAM', 'CAMAROES'),
('CAN', 'CANADA'),
('CAT', 'CATAR'),
('CAY', 'CAYMAN, ILHAS'),
('CAZ', 'CAZAQUISTAO, REPUBLICA DO'),
('CBJ', 'CAMBOJA'),
('CHA', 'CHADE'),
('CHI', 'CHILE'),
('CHN', 'CHINA, REPUBLICA POPULAR'),
('CHP', 'CHIPRE'),
('CHR', 'CHRISTMAS,ILHA (NAVIDAD)'),
('CIN', 'CINGAPURA'),
('CMF', 'COSTA DO MARFIM'),
('COC', 'COCOS(KEELING),ILHAS'),
('COD', 'COREIA, REP.POP.DEMOCRATICA'),
('COL', 'COLOMBIA'),
('COM', 'COMORES, ILHAS'),
('CON', 'CONGO'),
('COO', 'COOK, ILHAS'),
('COR', 'COREIA, REPUBLICA DA'),
('COS', 'COSTA RICA'),
('COV', 'COVEITE'),
('CPN', 'ZONA DO CANAL DO PANAMA'),
('CRO', 'CROACIA (REPUBLICA DA)'),
('CUB', 'CUBA'),
('DIN', 'DINAMARCA'),
('DJB', 'DJIBUTI'),
('DMC', 'REPUBLICA DOMINICANA'),
('DOM', 'DOMINICA,ILHA'),
('EGI', 'EGITO'),
('ELS', 'EL SALVADOR'),
('EMI', 'EMIRADOS ARABES UNIDOS'),
('EQU', 'EQUADOR'),
('ERI', 'ERITREIA'),
('ESC', 'ESLOVACA, REPUBLICA'),
('ESN', 'ESLOVENIA, REPUBLICA DA'),
('ESP', 'ESPANHA'),
('EST', 'ESTONIA, REPUBLICA DA'),
('ETI', 'ETIOPIA'),
('EUA', 'ESTADOS UNIDOS'),
('FAL', 'FALKLAND (ILHAS MALVINAS)'),
('FER', 'FEROE, ILHAS'),
('FIJ', 'FIJI'),
('FIL', 'FILIPINAS'),
('FIN', 'FINLANDIA'),
('FOR', 'FORMOSA (TAIWAN)'),
('FRA', 'FRANCA'),
('GAB', 'GABAO'),
('GAM', 'GAMBIA'),
('GAN', 'GANA'),
('GEO', 'GEORGIA, REPUBLICA DA'),
('GEQ', 'GUINE-EQUATORIAL'),
('GIB', 'GIBRALTAR'),
('GRA', 'GRANADA'),
('GRE', 'GRECIA'),
('GRO', 'GROENLANDIA'),
('GUA', 'GUADALUPE'),
('GUB', 'GUINE-BISSAU'),
('GUE', 'GUINE'),
('GUF', 'GUIANA FRANCESA'),
('GUI', 'GUIANA'),
('GUM', 'GUAM'),
('GUT', 'GUATEMALA'),
('HAI', 'HAITI'),
('HKG', 'HONG KONG'),
('HOL', 'PAISES BAIXOS (HOLANDA)'),
('HON', 'HONDURAS'),
('HUG', 'HUNGRIA, REPUBLICA DA'),
('IAV', 'VIRGENS,ILHAS (E.U.A.)'),
('IBV', 'VIRGENS,ILHAS (BRITANICAS)'),
('ICA', 'CANARIAS, ILHAS'),
('IDN', 'INDONESIA'),
('IEM', 'IEMEN'),
('IND', 'INDIA'),
('IRA', 'IRA, REPUBLICA ISLAMICA DO'),
('IRL', 'IRLANDA'),
('IRQ', 'IRAQUE'),
('ISL', 'ISLANDIA'),
('ISR', 'ISRAEL'),
('ITA', 'ITALIA'),
('JAM', 'JAMAICA'),
('JAP', 'JAPAO'),
('JER', 'JERSEY, ILHA DO CANAL'),
('JOR', 'JORDANIA'),
('JTN', 'JOHNSTON, ILHAS'),
('KBT', 'KIRIBATI'),
('LAO', 'LAOS, REP.POP.DEMOCR.DO'),
('LBN', 'LIBANO'),
('LBR', 'LIBERIA'),
('LCH', 'LIECHTENSTEIN'),
('LES', 'LESOTO'),
('LET', 'LETONIA, REPUBLICA DA'),
('LIB', 'LIBIA'),
('LIT', 'LITUANIA, REPUBLICA DA'),
('LXB', 'LUXEMBURGO'),
('MAC', 'MACAU'),
('MAD', 'MADEIRA, ILHA DA'),
('MAL', 'MALASIA'),
('MAN', 'MAN, ILHA DE'),
('MAR', 'MARROCOS'),
('MAU', 'MAURICIO'),
('MCB', 'MOCAMBIQUE'),
('MCD', 'MACEDONIA, ANT.REP.IUGOSLAVA'),
('MCN', 'MICRONESIA'),
('MDG', 'MADAGASCAR'),
('MDV', 'MALDIVAS'),
('MDW', 'MIDWAY, ILHAS'),
('MEX', 'MEXICO'),
('MIA', 'MIANMAR (BIRMANIA)'),
('MLI', 'MALI'),
('MLT', 'MALTA'),
('MLV', 'MALAVI'),
('MNG', 'MONGOLIA'),
('MOL', 'MOLDAVIA, REPUBLICA DA'),
('MON', 'MONACO'),
('MRN', 'MARIANAS DO NORTE'),
('MRS', 'MARSHALL,ILHAS'),
('MRT', 'MAURITANIA'),
('MSR', 'MONTSERRAT,ILHAS'),
('MTG', 'MONTENEGRO'),
('MTN', 'MARTINICA'),
('NAM', 'NAMIBIA'),
('NAU', 'NAURU'),
('NEP', 'NEPAL'),
('NGR', 'NIGER'),
('NIC', 'NICARAGUA'),
('NIG', 'NIGERIA'),
('NIU', 'NIUE,ILHA'),
('NOR', 'NORUEGA'),
('NRF', 'NORFOLK,ILHA'),
('OMA', 'OMA'),
('PAC', 'PACIFICO,ILHAS DO (POSSESSAO DOS EUA)'),
('PAL', 'PALAU'),
('PAN', 'PANAMA'),
('PAP', 'PAPUA NOVA GUINE'),
('PAQ', 'PAQUISTAO'),
('PAR', 'PARAGUAI'),
('PER', 'PERU'),
('PIT', 'PITCAIRN,ILHA'),
('PLN', 'POLINESIA FRANCESA'),
('POL', 'POLONIA, REPUBLICA DA'),
('POR', 'PORTUGAL'),
('PTR', 'PORTO RICO'),
('QUE', 'QUENIA'),
('QUI', 'QUIRGUIZ, REPUBLICA'),
('RCG', 'CONGO, REPUBLICA DEMOCRATICA DO'),
('REI', 'REINO UNIDO'),
('REU', 'REUNIAO, ILHA'),
('RK)', 'GUERNSEY, ILHA DO CANAL (INCLUI ALDERNEY E SA'),
('ROM', 'ROMENIA'),
('RUA', 'RUANDA'),
('RUS', 'RUSSIA, FEDERACAO DA'),
('SAA', 'SAARA OCIDENTAL'),
('SAI', 'SAINT KITTS E NEVIS'),
('SAL', 'SALOMAO, ILHAS'),
('SAM', 'SAMOA'),
('SCT', 'SAO CRISTOVAO E NEVES,ILHAS'),
('SER', 'SERVIA'),
('SEY', 'SEYCHELLES'),
('SIR', 'SIRIA, REPUBLICA ARABE DA'),
('SLE', 'SERRA LEOA'),
('SMA', 'SAMOA AMERICANA'),
('SMR', 'SAN MARINO'),
('SNE', 'SENEGAL'),
('SOM', 'SOMALIA'),
('SPD', 'SAO PEDRO E MIQUELON'),
('SRI', 'SRI LANKA'),
('STH', 'SANTA HELENA'),
('STL', 'SANTA LUCIA'),
('STM', 'SAO TOME E PRINCIPE, ILHAS'),
('SUA', 'SUAZILANDIA'),
('SUD', 'SUDAO'),
('SUE', 'SUECIA'),
('SUI', 'SUICA'),
('SUR', 'SURINAME'),
('SVC', 'SAO VICENTE E GRANADINAS'),
('TAD', 'TADJIQUISTAO, REPUBLICA DO'),
('TAI', 'TAILANDIA'),
('TAN', 'TANZANIA, REP.UNIDA DA'),
('TCH', 'TCHECA, REPUBLICA'),
('TCM', 'TURCOMENISTAO, REPUBLICA DO'),
('TER', 'TERRITORIO BRIT.OC.INDICO'),
('TIM', 'TIMOR LESTE'),
('TOG', 'T'),
('TON', 'TONGA'),
('TOQ', 'TOQUELAU,ILHAS'),
('TRI', 'TRINIDAD E TOBAGO'),
('TRQ', 'TURQUIA'),
('TUN', 'TUNISIA'),
('TUR', 'TURCAS E CAICOS,ILHAS'),
('TUV', 'TUVALU'),
('UCR', 'UCRANIA'),
('UGA', 'UGANDA'),
('URU', 'URUGUAI'),
('UZB', 'UZBEQUISTAO, REPUBLICA DO'),
('VAN', 'VANUATU'),
('VAT', 'VATICANO, EST.DA CIDADE DO'),
('VEN', 'VENEZUELA'),
('VIE', 'VIETNA'),
('WAK', 'WAKE, ILHA'),
('ZAM', 'ZAMBIA'),
('ZEL', 'NOVA ZELANDIA'),
('ZIM', 'ZIMBABUE');










